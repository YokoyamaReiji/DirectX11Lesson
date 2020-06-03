#pragma once 

class Missile
{
public:
	Missile();          //コンストラクタ
	~Missile();			//デストラクタ

	void Deserialize(); //デシリアライズ（初期化）
	void Update();		//更新
	void Draw();		//描画

	inline void SetMatrix(const KdMatrix& rMat) { m_mWorld = rMat; }
	inline bool IsAlive() const { return m_alive; }
	inline void Destroy() { m_alive = false; }

private:

	void Release();		//解放	

	kdModel*  m_pModel = nullptr;
	KdMatrix  m_mWorld;
	float     m_speed = 0.5f;
	bool	  m_alive = true;
	int		  m_lifeSpan = 0;
};