﻿#pragma once

#include"../GameObject.h"

class Missile : public GameObject
{
public:
	void Deserialize();	//初期化
	void Update();		//更新
	
private:
	
	float	 m_speed = 0.5f;	
	int		 m_lifeSpan = 0;
};
