#pragma once

#include"../GameObject.h"

class Missile : public GameObject
{
public:
	void Deserialize(const json11::Json& jsonObj) override;	//初期化
	void Update();		//更新
	
	inline void SetTarget(const std::shared_ptr<GameObject>& spTarget) { m_wpTarget = spTarget; }

	//当たり判定の更新
	void UpdateCollision();

	//撃ったキャラクターを登録する
	inline void SetOwner(const std::shared_ptr<GameObject>& spOwner) { m_wpOwner = spOwner; }

	void Explosion();

	void UpdateTrail();			//軌跡の更新
	void DrawEffect()override;  //透明物の描画

private:
	
	KdVec3 m_prevPos;//1フレーム前の座標

	std::weak_ptr<GameObject> m_wpOwner;//発射したオーナーオブジェクト

	float	 m_speed = 0.5f;	
	int		 m_lifeSpan = 0;
	std::weak_ptr<GameObject> m_wpTarget;

	int m_attackPow = 5;

	//煙・奇跡
	KdTrailPolygon   m_trailSmoke;
	float m_trailRotate = 0.0f;
};

