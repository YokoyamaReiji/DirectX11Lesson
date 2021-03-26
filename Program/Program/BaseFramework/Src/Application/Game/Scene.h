#pragma once

#include "EditorCamera.h"

//前方宣言
class GameObject;
class CameraComponent;

class Scene
{
public:
	static Scene& GetInstance()
	{
		static Scene instance;
		return instance;
	}

	~Scene();	//デストラクタ

	void Init();	//初期化
	void Deserialize();

	//シーン変更のリクエストを受付
	void RequestChangeScene(const std::string& fileName);

	void Release();	//解放
	void Update();	//更新
	void Draw();	//描画

	//GameObjectのリストを返す
	const std::list<std::shared_ptr<GameObject>> GetObjects()const { return m_spObjects; }

	void AddObject(std::shared_ptr<GameObject> spObject);

	inline void SetTargetCamera(std::shared_ptr<CameraComponent> spCamera) { m_wpTargetCamera = spCamera; }

	void ImGuiUpdate();//ImGuiの更新

	//デバックライン描画
	void AddDebugLine(const Math::Vector3& p1, const Math::Vector3& p2, const Math::Color& color = { 1,1,1,1 });

	//デバックスフィア描画
	void AddDebugSphereLine(const Math::Vector3& pos, float radius, const Math::Color& color = { 1,1,1,1 });

	//軸描画
	void AddDebugCoorrdinateAxisLine(const Math::Vector3& pos, float scale = 1.0f);
private:
	Scene();	//コンストラクタ

	void LoadScene(const std::string& sceneFilename);

	void ExecChangeScene();				//シーンを実際に変更するところ
	void Reset();						//シーンをまたぐときにリセットする処理

	std::string m_nextSceneFileName = ""; //次のシーンのjsonファイル名
	bool m_isRequestChangeScene = false;  //シーン移動のリクエストがあったかどうか

	std::shared_ptr<KdModel> m_spSky = nullptr;						//スカイスフィア
	std::shared_ptr<EditorCamera> m_spCamera = nullptr;

	bool		  m_editorCameraEnable = true;

	std::list<std::shared_ptr<GameObject>> m_spObjects;

	//Imguiで選択されたオブジェクト
	std::weak_ptr<GameObject>  m_wpImguiSelectObj;

	//ターゲットカメラ
	std::weak_ptr<CameraComponent> m_wpTargetCamera;

	//デバックライン描画用の頂点配列
	std::vector<KdEffectShader::Vertex> m_debugLines;

};

