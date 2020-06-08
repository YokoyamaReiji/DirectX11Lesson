#pragma once

#include "EditorCamera.h"

//前方宣言
class GameObject;

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

	void AddObject(GameObject* pObject);

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
	bool          m_edtorCameraEnable = true;

	std::list<GameObject*> m_objects;

	//デバックライン描画用の頂点配列
	std::vector<KdEffectShader::Vertex> m_debugLines;

};