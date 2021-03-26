#pragma once

#include"../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj)override;	//初期化：オブジェクト作成用外部データの解釈
	void Update();		//更新

	//相手からダメージ通知を受ける関数
	void OnNotify_Damage(int damage);

	void Draw() override;//描画

	void DrawEffect() override;//透明物描画

private:

	void UpdateMove();	//移動更新
	void UpdateShoot(); //発射関数
	void UpdateCollision();//当たり判定処理
	void UpdatePropeller();//プロペラ更新
	
	//std::shared_ptr<GameObject> m_spPropeller;//プロペラ用GameObject

	//プロペラが飛行機本体からどれだけ離れているか
	//KdMatrix m_mPropLocal;

	//プロペラの回転速度
	float m_propRotSpeed = 0.0f;

	KdTrailPolygon m_propTrail;

	float m_speed = 0.2f;	//移動スピード
	bool m_canshoot = true;//発射可能かどうか

	KdVec3 m_prevPos;//１フレーム前の座標

	bool  m_laser = false;			//レーザー発射
	float m_laserRange = 1000.0f;	//レーザーの射程

	int m_hp = 10;//０になるとリストから抜く

	//基礎アクションステート
	class BaseAction
	{
	public:
		virtual void Update(Aircraft& owner) = 0;
	};

	//飛行中
	class ActionFly : public BaseAction
	{
	public:
		virtual void Update(Aircraft& owner)override;
	};

	//墜落中
	class ActionCrash : public BaseAction
	{
	public:
		virtual void Update(Aircraft& owner)override;

		int m_timer = 180;
	};

	//現在実行するアクションステート
	std::shared_ptr<BaseAction> m_spActionState;
};

