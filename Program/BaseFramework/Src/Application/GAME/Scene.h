#pragma once

#include "EditorCamera.h"

//前方宣言
class StageObject;
class Aircraft;
class Missile;

class Scene
{
public:
	static Scene& GetInstance()
	{
		static Scene instance;
		return instance;
	}

	~Scene();//デストラクタ

	void Init();
	void Release();
	void Update();
	void Draw();

	void AddMissile(Missile* pMissile);

	void ImGuiUpdate();

	//デバックライン描画
	void AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color = { 1,1,1,1 });

	//デバックスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	//デバック座標軸描画
	void AddDebugCoordinateAxisLine(const Math::Vector3& pos, float scale = 1.0f);

private:

	Scene();

	kdModel       m_sky;//スカイスフィア
	EditorCamera  m_camera;
	StageObject*  m_pGround = nullptr;//ステージの地面
	Aircraft*     m_pAircraft = nullptr;//飛行機
	bool          m_edtorCameraEnable = true;

	std::list<Missile*> m_missiles;

	//デバックライン描画用の頂点配列
	std::vector<KdEffectShader::Vertex> m_debugLines;

};