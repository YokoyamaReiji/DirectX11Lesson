﻿#pragma once

#include "../Game/GameObject.h"

//=========================================
//カメラコンポーネントクラス
//==========================================
class CameraComponent
{
public:
	//コンストラクタ　オーナーの設定と射影行列の作成
	CameraComponent(GameObject& owner);
	~CameraComponent();

	//オフセット行列取得
	inline KdMatrix& OffseMatrix() { return m_mOffset; }

	//カメラ行列取得
	inline const KdMatrix& GetCameraMatrix() { return m_mCam; }

	//ビュー行列取得
	inline const KdMatrix& GetViewMatrix() { return m_mView; }

	//カメラ行列・ビュー行列設定(行列ｍと行列Offsetが合成され、最終的なカメラ行列になる)
	void SetCameraMatrix(const KdMatrix& m);

	//カメラ情報(ビュー・射影行列など)をシェーダーにセット
	void SetToShader();

private:

	//オフセット行列
	KdMatrix    m_mOffset;
	//カメラ行列
	KdMatrix    m_mCam;
	//ビュー行列
	KdMatrix    m_mView;
	//射影行列
	KdMatrix    m_mProj;

	GameObject& m_owner;

};
