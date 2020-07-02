#pragma once

#include"../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize(const json11::Json& jsonObj)override;	//初期化：オブジェクト作成用外部データの解釈
	void Update();		//更新

	void UpdateMove();	//移動更新

	void UpdateShoot(); //発射関数

	void ImGuiUpdate(); //Aircraftのimgui更新

	void UpdateCollision();//当たり判定処理
private:
	
	float m_speed = 0.2f;	//移動スピード
	bool m_canshoot = true;//発射可能かどうか

	KdVec3 m_prevPos = {};//１フレーム前の座標
};

