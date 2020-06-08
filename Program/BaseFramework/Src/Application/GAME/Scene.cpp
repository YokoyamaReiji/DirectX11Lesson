#include"Scene.h"

#include "GameObject.h"

#include "Shooting/StageObject.h"
#include "Shooting/Aircraft.h"
#include "Shooting/Missile.h"

//コンストラクタ
Scene::Scene()
{

}

//デストラクタ
Scene::~Scene()
{

}

//初期化
void Scene::Init()
{
	m_sky.Load("Data/Sky/Sky.gltf");

	StageObject* pGround = new StageObject();
	if (pGround)
	{
		pGround->Deserialize();
		m_objects.push_back(pGround);
	}

	Aircraft* pAircraft = new Aircraft();
	if (pAircraft)
	{
		pAircraft->Deserialize();
		m_objects.push_back(pAircraft);
	}
}

void Scene::Release()
{
	for (auto pObject : m_objects)
	{
		delete pObject;
	}
	m_objects.clear();
}

void Scene::Update()
{
	
	if (m_edtorCameraEnable)
	{
		m_camera.Update();
	}
	
	for (auto pObject : m_objects)
	{
		pObject->Update();
	}

	for (auto pObjectItr = m_objects.begin(); pObjectItr != m_objects.end();)
	{
		//寿命が尽きていたらリストから除外
		if ((*pObjectItr)->IsAlive()==false)
		{
			delete(*pObjectItr);
			pObjectItr = m_objects.erase(pObjectItr);
		}
		else
		{
			**pObjectItr;
		}
	}
}

void Scene::Draw()
{
	//エディターカメラをシェーダーにセット
	if (m_edtorCameraEnable)
	{
		m_camera.SetToShader();
	}
	else
	{

	}

	
	//カメラ情報（ビュー行列、射影行列）を、各シェーダの定数バッファにセット
	//SHADER.m_cb7_Camera.Write();



	//ライトの情報をセット
	SHADER.m_cb8_Light.Write();

	//エフェクトシェーダーを描画デバイスをセット
	SHADER.m_effectShader.SetToDevice();

	//
	Math::Matrix skyScale = DirectX::XMMatrixScaling(100.0f, 100.0f, 100.0f);

	SHADER.m_effectShader.SetWorldMatrix(skyScale);

	//モデルの描画（メッシュ情報とマテリアル情報を渡す）
	SHADER.m_effectShader.DrawMesh(m_sky.GetMesh(), m_sky.GetMaterials());

	//不透明物描画
	SHADER.m_standardShader.SetToDevice();

	for (auto pObject : m_objects)
	{
		pObject->Draw();
	}

	//デバックライン描画
	SHADER.m_effectShader.SetToDevice();
	SHADER.m_effectShader.SetTexture(D3D.GetWhiteTex()->GetSRView());
	{
		AddDebugLine(Math::Vector3(), Math::Vector3(0.0f, 10.0f, 0.0f));

		AddDebugSphereLine(Math::Vector3(5.0f, 5.0f, 0.0f),2.0f);

		AddDebugCoordinateAxisLine(Math::Vector3(0.0f, 5.0f, 5.0f), 3.0f);

		//Zバッファ使用OFF：書き込みOFF
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZDisable_ZWriteDisable, 0);


		/*std::vector<KdEffectShader::Vertex>debugLinse;

		//ラインの開始地点
		KdEffectShader::Vertex ver1;
		ver1.Color = { 1.0f,1.0f,1.0f,1.0f };
		ver1.UV = { 0.0f,0.0f};
		ver1.Pos = { 0.0f,0.0f,0.0f};

		//ラインの終端地点
		KdEffectShader::Vertex ver2;
		ver2.Color = { 1.0f,1.0f,1.0f,1.0f };
		ver2.UV = { 0.0f,0.0f };
		ver2.Pos = { 0.0f,1.0f,0.0f };

		debugLinse.push_back(ver1);
		debugLinse.push_back(ver2);*/
		if (m_debugLines.size() >= 1)
		{
			SHADER.m_effectShader.SetWorldMatrix(Math::Matrix());

			SHADER.m_effectShader.DrawVertices(m_debugLines, D3D_PRIMITIVE_TOPOLOGY_LINELIST);

			m_debugLines.clear();
		}

		//Zバッファ使用ON：書き込みON
		D3D.GetDevContext()->OMSetDepthStencilState(SHADER.m_ds_ZEnable_ZWriteEnable, 0);
	}
}

void Scene::AddObject(GameObject* pObject)
{
	if (pObject == nullptr) { return; }
	m_objects.push_back(pObject);
}

void Scene::ImGuiUpdate()
{
	if (ImGui::Begin("Scene"))
	{
		//ImGui::Text(u8"今日はいい天気だから\n飛行機の座標でも表示しようかな");
		ImGui::Checkbox("EditorCamera", &m_edtorCameraEnable);

		Aircraft* pAircraft = new Aircraft();
		if (pAircraft)
		{
			pAircraft->ImGuiUpdate();
		}
	}

	ImGui::End();

}

//デバックライン描画
void Scene::AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color)
{
	//ラインの開始地点
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f,0.0f };
	ver.Pos = p1;
	m_debugLines.push_back(ver);


	//ラインの終端地点
	ver.Pos = p2;
	m_debugLines.push_back(ver);
}

//デバックスフィア描画
void Scene::AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color)
{
	KdEffectShader::Vertex ver;
	ver.Color = color;
	ver.UV = { 0.0f,0.0f };

	static constexpr int kDetail = 32;
	for (UINT i = 0; i < kDetail + 1; i++)
	{
		//XZ平面
		ver.Pos = pos;
		ver.Pos.x += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		//XY平面
		ver.Pos = pos;
		ver.Pos.x += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.y += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.x += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.y += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		//YZ平面
		ver.Pos = pos;
		ver.Pos.y += cos((float)i * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)i * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);

		ver.Pos = pos;
		ver.Pos.y += cos((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		ver.Pos.z += sin((float)(i + 1) * (360 / kDetail) * KdToRadians) * radius;
		m_debugLines.push_back(ver);
	}
}

//デバック座標軸描画
void Scene::AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale )
{
	KdEffectShader::Vertex ver;
	ver.Color = { 1.0f,1.0f,1.0f,1.0f };
	ver.UV = { 0,0 };

	//X軸・赤
	ver.Color = { 1.0f,0.0f,0.0f,1.0f };
	ver.Pos = pos;
	m_debugLines.push_back(ver);

	ver.Pos.x += 1.0f * scale;
	m_debugLines.push_back(ver);

	//Y軸・緑
	ver.Color = { 0.0f,1.0f,0.0f,1.0f };
	ver.Pos = pos;
	m_debugLines.push_back(ver);

	ver.Pos.y += 1.0f * scale;
	m_debugLines.push_back(ver);

	//Z軸・青
	ver.Color = { 0.0f,0.0f,1.0f,1.0f };
	ver.Pos = pos;
	m_debugLines.push_back(ver);

	ver.Pos.z += 1.0f * scale;
	m_debugLines.push_back(ver);
}

