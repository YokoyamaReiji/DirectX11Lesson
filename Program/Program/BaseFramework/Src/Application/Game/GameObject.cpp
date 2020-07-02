#include "GameObject.h"
#include "../Component/CameraComponent.h"
#include "../Component/InputComponent.h"
#include "../Component/ModelComponent.h"
#include "Shooting/Aircraft.h"

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

bool GameObject::HitCheckBySphere(const SphereInfo& rInfo)
{
	//当たっているとする距離の計算(お互いの半径を足した値)
	float hitRange = rInfo.m_radius + m_colRadiud;

	//自分の座標ベクトル
	KdVec3 myPos = m_mWorld.GetTranslation();

	//2点間のベクトルを計算
	KdVec3 betweenVec = rInfo.m_pos - myPos;

	//２点間の距離の計算
	float distance = betweenVec.Lenght();

	bool isHit = false;
	if (distance <= hitRange)
	{
		isHit = true;
	}

	return isHit;
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
	else if (name == "Aircraft")
	{
		return std::make_shared<Aircraft>();
	}

	//文字列が既存のクラスに一致しなかった
	assert(0 && "存在しないGameObjectクラスです");
	return nullptr;
}
