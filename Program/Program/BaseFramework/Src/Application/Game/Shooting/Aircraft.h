﻿#pragma once

#include"../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:

	void Deserialize();	//初期化：オブジェクト作成用外部データの解釈
	void Update();		//更新

	void UpdateMove();	//移動更新

	void UpdateShoot(); //発射関数

	void ImGuiUpdate(); //Aircraftのimgui更新
private:
	
	float m_speed = 0.2f;	//移動スピード
	bool m_canshoot = true;//発射可能かどうか
};

