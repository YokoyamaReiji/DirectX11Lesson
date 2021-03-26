#pragma once

#include "GameObject.h"

//ゲームのシーンの管理者をカテゴリ分けするために,GameProcessにGameObjectを継承させる
class GameProcess : public GameObject
{
public:
	GameProcess() {}
	virtual ~GameProcess(){}
};