#pragma once

class CameraComponent;
class InputComponent;
class ModelComponent;

//タグ定数
enum OBJECT_TAG
{
	TAG_None      = 0x00000000,	//属性なし：初期設定用
	TAG_Cheracter = 0x00000001,	//キャラクター設定
	TAG_Player    = 0x00000002,	//プレイヤー属性
};

class GameObject : public std::enable_shared_from_this<GameObject>
{
public:
	GameObject();
	virtual ~GameObject();

	virtual void Deserialize(const json11::Json& jsonObj);	//初期化
	virtual void Update();		//更新
	virtual void Draw();		//描画

	inline const KdMatrix& GetMatrix()const { return m_mWorld; }
	inline void SetMatrix(const KdMatrix& rMat) { m_mWorld = rMat; }
	inline bool IsAlive() const { return m_alive; }
	inline void Destroy() { m_alive = false; }

	inline void SetTag(UINT tag) { m_tag; }
	inline UINT GetTag()const { return m_tag; }

	//カメラコンポーネント取得
	std::shared_ptr<CameraComponent> GetCameraComponent() { return m_spCameraComponent; }
	


protected:
	virtual void Release();

	//カメラコンポーネント
	std::shared_ptr<CameraComponent> m_spCameraComponent = std::make_shared<CameraComponent>(*this);
	//インップトコンポーネント
	std::shared_ptr<InputComponent> m_spInputComponent = std::make_shared<InputComponent>(*this);
	//モデルコンポーネント
	std::shared_ptr<ModelComponent> m_spModelComponent = std::make_shared<ModelComponent>(*this);

	std::shared_ptr<KdModel> m_pModel = nullptr;
	KdMatrix m_mWorld;
	bool	 m_alive = true;

	UINT   m_tag = OBJECT_TAG::TAG_None;
	std::string m_name = "GameObject";

};

