#include "ShootingGaameProcess.h"
#include "../Scene.h"

void ShootingGameProcess::Update()
{
	if (GetAsyncKeyState(VK_RETURN) & 0x8000)
	{
		//ダメ！キャラクターリストを回している最中にリストをリセットしようとしてる
		//Scene::GetInstance().LoadScene("Data/Scene/ActionGame.json");

		//シーンのリクエストを受けてから
		Scene::GetInstance().RequestChangeScene("Data/Scene/ActionGame.json");
	}
}
