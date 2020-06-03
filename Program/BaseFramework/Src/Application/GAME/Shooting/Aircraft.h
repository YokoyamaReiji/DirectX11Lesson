#pragma once

class Missile;

class Aircraft
{
public:
	Aircraft();    //コンストラクタ
	~Aircraft();   //デストラクタ
	
	void Deserialize();//初期化：オブジェクト作成用外部データの解釈
	void Update();	   //更新
	void Draw();       //描画

	void SetCameraToShader();//追従カメラビュー行列、射影行列をシェーダーにセット

	void UpdateMove();//移動更新処理
	void UpdateShoot();//発射関数

	void ImGuiUpdate();//Aircraftクラス専用のIMGUI更新

private:

	void Release();

	kdModel*      m_pModel = nullptr;//3Dモデルデータ
	KdMatrix  m_mWorld;//飛行機のモデル行列

	float m_speed = 0.2f;//移動スピード

	KdMatrix   m_mOffset;//追従カメラからの相対座標行列
	Math::Matrix   m_mPosj;//追従カメラの射影行列

	bool			m_canShoot = true;		//発射可能かどうか
};