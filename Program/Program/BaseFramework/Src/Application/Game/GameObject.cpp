#include "GameObject.h"
#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"
#include "../Component/ModelComponent.h"
#include "Shooting/Aircraft.h"
#include "Shooting/ShootingGaameProcess.h"

GameObject::GameObject()
{
}

GameObject::~GameObject()
{
	Release();
}

void GameObject::Deserialize(const json11::Json& jsonObj)
{
	if (jsonObj.is_null()) { return; }

	//名前
	if (jsonObj["Name"].is_null() == false)
	{
		m_name = jsonObj["Name"].string_value();
	}

	//タグ
	if (jsonObj["Tag"].is_null() == false)
	{
		m_tag = jsonObj["Tag"].int_value();
	}

	//モデル=================
	if (m_spModelComponent)
	{
		m_spModelComponent->SetModel(KdResFac.GetModel(jsonObj["ModelFileName"].string_value()));
	}

	//行列=========================
	KdMatrix mTrans,mRotate,mScale;

	//座標
	const std::vector<json11::Json>& rPos = jsonObj["Pos"].array_items();
	if (rPos.size() == 3)
	{
		mTrans.CreateTranslation((float)rPos[0].number_value(), (float)rPos[1].number_value(),
			(float)rPos[2].number_value());
	}

	//回転
	const std::vector<json11::Json>& rRot = jsonObj["Rot"].array_items();
	if (rPos.size() == 3)
	{
		mTrans.CreateRotationX((float)rRot[0].number_value() * KdToRadians);
		mTrans.RotateY((float)rRot[1].number_value() * KdToRadians);
		mTrans.RotateZ((float)rRot[2].number_value() * KdToRadians);
	}
	
	//座標
	const std::vector<json11::Json>& rScale = jsonObj["Scale"].array_items();
	if (rPos.size() == 3)
	{
		mScale.CreatScalling((float)rPos[0].number_value(), (float)rPos[1].number_value(),
			(float)rPos[2].number_value());
	}

	m_mWorld = mScale * mRotate * mTrans;

}

void GameObject::Update(){}

void GameObject::Draw()
{
	if (m_spModelComponent == nullptr) { return; }

	m_spModelComponent->Draw();
}

void GameObject::DrawEffect()
{
}

void GameObject::ImGuiUpdate()
{
	ImGui::InputText("Name", &m_name);

	//TAG
	if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::CheckboxFlags("Character", &m_tag, TAG_Cheracter);
		ImGui::CheckboxFlags("Player", &m_tag, TAG_Player);
		ImGui::CheckboxFlags("StageObject", &m_tag, TAG_StageObject);
		ImGui::CheckboxFlags("AttackHit", &m_tag, TAG_AttacHit);

		if (ImGui::Button(u8"JSONテキストコピー"))
		{
			ImGui::SetClipboardText(KdFormat("\"Tag\": %d", m_tag).c_str());
		}
		ImGui::TreePop();
	}
	
	//Transform
	if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen))
	{
		KdVec3 pos = m_mWorld.GetTranslation();
		KdVec3 rot = m_mWorld.GetAngles() * KdToDegrees;

		bool isChange = false;

		isChange |= ImGui::DragFloat3("Position", &pos.x, 0.01f);
		isChange |= ImGui::DragFloat3("Rotation", &rot.x, 0.1f);

		if (isChange)
		{
			//計算するときはRadianに戻す
			rot *= KdToRadians;

			KdMatrix mR;
			mR.RotateX(rot.x);
			mR.RotateY(rot.y);
			mR.RotateZ(rot.z);

			m_mWorld = mR;

			m_mWorld.SetTranslation(pos);
		}
		if (ImGui::Button(u8"JSONテキストコピー"))
		{
			std::string s = KdFormat("\"Pot\":[%.1f,%.1f,%.1f],\n", pos.x, pos.y, pos.z);
			s += KdFormat("\"Rot\": [%.1f,%.1f,%.1f],\n", rot.x, rot.y, rot.z);
			ImGui::SetClipboardText(s.c_str());
		}
		ImGui::TreePop();
	}
}

bool GameObject::HitCheckBySphere(const SphereInfo& rInfo)
{
	//当たっているとする距離の計算(お互いの半径を足した値)
	float hitRange = rInfo.m_radius + m_colRadiud;

	//自分の座標ベクトル
	KdVec3 myPos = m_mWorld.GetTranslation();

	//2点間のベクトルを計算
	KdVec3 betweenVec = rInfo.m_pos - myPos;

	//２点間の距離の計算
	float distance = betweenVec.Length();

	bool isHit = false;
	if (distance <= hitRange)
	{
		isHit = true;
	}

	return isHit;
}

bool GameObject::HitCheckByRay(const RayInfo& rInfo, KdRayResult& rResult)
{
	//判定をする対象のモデルがない場合は当たってないとして帰る
	if (!m_spModelComponent) { return false; }

	//すべてのノード(メッシュ分当たり判定を行う)
	for (auto& node : m_spModelComponent->GetNodes())
	{
		KdRayResult tmpResult;//結果返送用

		//レイ判定(本体からのズレ分も加味して計算)
		KdRayToMesh(rInfo.m_pos, rInfo.m_dir, rInfo.m_maxRange, *(node.m_spMesh),
			node.m_localTransform * m_mWorld, tmpResult);

		//より近い判定を優先する
		if (tmpResult.m_distance < rResult.m_distance)
		{
			rResult = tmpResult;
		}
	}
	return rResult.m_hit;

	/*
	//モデルの逆行列でレイを変換
	KdMatrix invMat = m_mWorld;
	invMat.Inverse();//逆行列

	//レイの判定開始位置を逆変換
	KdVec3 rayPos = rInfo.m_pos;
	rayPos.TransformCoord(invMat);

	//発射方向は正規化されていないと正しく判定できないので正規化
	KdVec3 rayDir = rInfo.m_dir;
	rayDir.TransformNormal(invMat);//回転だけ

	//逆行列に拡縮が入っていると
	//レイが当たった距離にも拡縮が反映されてしまうので
	//判定用の最大距離にも拡縮を反映させておく
	float rayCheckRange = rInfo.m_maxRange * rayDir.Lenght();

	rayDir.Normalize();

	//面情報の取得
	const std::shared_ptr<KdMesh>& mesh = m_spModelComponent->GetMesh();//モデル情報の取得
	const KdMeshFace* pFaces = &mesh->GetFaces()[0];//面情報の先頭を取得
	UINT faceNum = mesh->GetFaces().size();

	//すべての面（三角形）と当たり判定
	for (UINT faceIdx = 0; faceIdx < faceNum; faceIdx++)
	{
		//三角形を構成する3つの頂点のindex
		const UINT* idx = pFaces[faceIdx].Idx;

		//レイと三角形の当たり判定
		float triDist = FLT_MAX;
		bool bHit = DirectX::TriangleTests::Intersects(
			//rInfo.m_pos,	
			rayPos,			//発射場所
			rayDir,			//発射方向

			//判定する3角形の頂点情報
			mesh->GetVertexPositions()[idx[0]],
			mesh->GetVertexPositions()[idx[1]],
			mesh->GetVertexPositions()[idx[2]],

			triDist//当たった場合の距離
			);
		//ヒットしていなかったらスキップ
		if (bHit == false) { continue; }

		//最大距離以内か
		if (triDist <= rayCheckRange)
		{
			return true;
		}
	}
	return false;*/
}

void GameObject::Release()
{
	
}

std::shared_ptr<GameObject> CreateGameObject(const std::string& name)
{
	if (name == "GameObject")
	{
		return std::make_shared<GameObject>();
	}
	if (name == "Aircraft")
	{
		return std::make_shared<Aircraft>();
	}
	if (name == "ShootingGameProcess")
	{
		return std::make_shared<ShootingGameProcess>();
	}

	//文字列が既存のクラスに一致しなかった
	assert(0 && "存在しないGameObjectクラスです");
	return nullptr;
}
