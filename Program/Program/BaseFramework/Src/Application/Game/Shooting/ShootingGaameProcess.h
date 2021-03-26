#pragma once
#include "../GameProcess.h"

//シューティングゲームシーンの管理と画面遷移を担当するクラス

//GameObject->ShoootingGameProcess
class ShootingGameProcess : public GameProcess
{
public:
	ShootingGameProcess(){}
	virtual ~ShootingGameProcess(){}

	void  Update() override;
};