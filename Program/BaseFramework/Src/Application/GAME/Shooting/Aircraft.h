#pragma once

#include "../GameObject.h"

class Missile;

class Aircraft : public GameObject
{
public:
	
	void Deserialize();//初期化：オブジェクト作成用外部データの解釈
	void Update();	   //更新

	void SetCameraToShader();//追従カメラビュー行列、射影行列をシェーダーにセット

	void UpdateMove();//移動更新処理
	void UpdateShoot();//発射関数

	void ImGuiUpdate();//Aircraftクラス専用のIMGUI更新

private:

	float m_speed = 0.2f;//移動スピード

	KdMatrix   m_mOffset;//追従カメラからの相対座標行列
	Math::Matrix   m_mPosj;//追従カメラの射影行列

	bool			m_canShoot = true;		//発射可能かどうか
};