#pragma once

class EditorCamera
{
public:

	EditorCamera();
	~EditorCamera();

	void Update();
	void SetToShader();
 
private:

	Math::Vector3  m_viewPos;//注意点
	Math::Matrix   m_mCam;//注意点からの行列
	Math::Matrix   m_mProj;//射影行列

	POINT          m_prevMousePos = { 0,0 };//前フレームマウスの座標
};