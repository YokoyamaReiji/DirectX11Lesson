#pragma once

//3Dベクトル
class KdVec3 : public DirectX::XMFLOAT3
{
public:

	//指定行列でVectorを追加する
	KdVec3& TransformCoord(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformCoord(*this, m);
		return *this;
	}

	//指定（回転）行列でVectorを変換する
	KdVec3& TransformNormal(const DirectX::XMMATRIX& m)
	{
		*this = XMVector3TransformNormal(*this, m);
		return *this;
	}

	//デフォルトコンストラクタは座標の０クリアを行う
	KdVec3()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

	//座標指定付きコンストラクタ(保存用)
	KdVec3(float _x, float _y, float _z) 
	{
		x = _x;
		y = _y;
		z = _z;
	}

	//XMVECTORから代入してきた時(計算用)
	KdVec3(const DirectX::XMVECTOR& v)
	{
		//変換して代入
		DirectX::XMStoreFloat3(this,v);
		//下記と同じだが、上記のSIMD命令のほうが高速
	/*	x = v.m128_f32[0];
		y = v.m128_f32[1];
		z = v.m128_f32[2];*/
	}

	//XMVECTORに変換(上記の関数の逆版)
	operator DirectX::XMVECTOR()const { return DirectX::XMLoadFloat3(this); }

	//Math::Vector3と互換性を持つ関数(自らのポインタをVector3に変換)
	operator Math::Vector3& () { return *(Math::Vector3*)this; }

	//算術演算子　乗算(*)
	KdVec3& operator*=(float s) 
	{
		*this = DirectX::XMVectorScale(*this,s);
		return*this;
	}

	//自分を正規化(元の名前が長いので短縮)
	void Normalize()
	{
		*this = DirectX::XMVector3Normalize(*this);
	}

	//ベクトルの長さ
	float Length() const 
	{
		return DirectX::XMVector3Length(*this).m128_f32[0];
	}

	//ベクトルの長さの2乗(高速なので判定用に使用する事が多い)
	float LenghtSquared() const
	{
		return DirectX::XMVector3LengthSq(*this).m128_f32[0];
	}

	//内積
	static float Dot(const KdVec3& v1, const KdVec3& v2)
	{
		return DirectX::XMVector3Dot(v1, v2).m128_f32[0];
	}

	//外積
	static KdVec3 Cross(const KdVec3& v1, const KdVec3& v2)
	{
		return DirectX::XMVector3Cross(v1, v2);
	}

	inline void Complement(const KdVec3& vTo, float rot);

};

//4*4の行列
class KdMatrix:public DirectX::XMFLOAT4X4
{
public:
	//デフォルトコンストラクタは単位行列化()
	KdMatrix()
	{
		*this = DirectX::XMMatrixIdentity();
	}

	//XMMATRIXから代入してきた
	KdMatrix(const DirectX::XMMATRIX& m)
	{
		DirectX::XMStoreFloat4x4(this, m);
	}

	//XMFLOAT4X4,Math::Matrixから代入してきた
	KdMatrix(const DirectX::XMFLOAT4X4& m) 
	{
		memcpy_s(this, sizeof(float) * 16, &m, sizeof(XMFLOAT4X4));
	}

	//XMMATRIXへ変換
	operator DirectX::XMMATRIX() const
	{
		return DirectX::XMLoadFloat4x4(this);
	}

	//Math::Matrixと互換性を持つ為の関数
	operator Math::Matrix& ()
	{
		return *(Math::Matrix*)this;
	}

	//代入演算子　乗算
	KdMatrix& operator*= (const KdMatrix& m)
	{
		*this = DirectX::XMMatrixMultiply(*this, m);
		return *this;
	}

	//作成============================================

	//移動行列作成
	void CreateTranslation(float x,float y,float z)
	{
		*this = DirectX::XMMatrixTranslation(x, y, z);
	}

	//X回転行列作成
	void CreateRotationX(float angle) 
	{
		*this = DirectX::XMMatrixRotationX(angle);
	}

	//拡縮行列作成
	void CreatScalling(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
	}

	//指定軸回転行列作成
	void CreateRotetionAxis(const KdVec3& axis, float angle)
	{
		*this = DirectX::XMMatrixRotationAxis(axis, angle);
	}

	//透視影行列の作成
	KdMatrix& CreateProjectionPerspectFov(float fovAngleY, float aspectRatio, float nearZ, float farZ)
	{
		*this = DirectX::XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, nearZ, farZ);
		return *this;
	}

	//操作============================================

	//X軸回転
	void RotateX(float angle)
	{
		*this *= DirectX::XMMatrixRotationX(angle);
	}

	//Z軸回転
	void RotateZ(float angle) 
	{
		*this *= DirectX::XMMatrixRotationZ(angle);
	}

	//Y軸回転
	void RotateY(float angle)
	{
		*this *= DirectX::XMMatrixRotationY(angle);
	}

	//拡縮
	void Scale(float x, float y, float z)
	{
		*this = DirectX::XMMatrixScaling(x, y, z);
	}

	//座標の移動関数
	void Move(const KdVec3& r)
	{
		_41 += r.x;
		_42 += r.y;
		_43 += r.z;
	}

	// 逆行列にする
	void Inverse()
	{
		*this = DirectX::XMMatrixInverse(nullptr, *this);
	}

	//プロパティ======================================

	//X軸取得
	KdVec3 GetAxisX() const { return{ _11,_12,_13 }; }

	//Y軸取得
	KdVec3 GetAxisY() const { return{ _21,_22,_23 }; }

	//Z軸取得
	KdVec3 GetAxisZ() const { return{ _31,_32,_33 }; }

	//X軸セット
	void SetAxisX(const KdVec3& v)
	{
		_11 = v.x;
		_12 = v.y;
		_13 = v.z;
	}

	//Y軸セット
	void SetAxisY(const KdVec3& v)
	{
		_21 = v.x;
		_22 = v.y;
		_23 = v.z;
	}

	//Z軸セット
	void SetAxisZ(const KdVec3& v)
	{
		_31 = v.x;
		_32 = v.y;
		_33 = v.z;
	}

	//座標取得
	KdVec3 GetTranslation() const { return{ _41,_42,_43 }; }

	//座標セット
	void SetTranslation(const KdVec3& v)
	{
		_41 = v.x;
		_42 = v.y;
		_43 = v.z;
	}

	//XYZの順番で合成したときの、回転速度を算出する
	KdVec3 GetAngles() const
	{
		KdMatrix mat = *this;

		//各軸を取得
		KdVec3 axisX = mat.GetAxisX();
		KdVec3 axisY = mat.GetAxisY();
		KdVec3 axisZ = mat.GetAxisZ();

		//各軸を正規化
		axisX.Normalize();
		axisY.Normalize();
		axisZ.Normalize();

		//マトリックスを正規化
		mat.SetAxisX(axisX);
		mat.SetAxisY(axisY);
		mat.SetAxisZ(axisZ);

		KdVec3 angles;
		angles.x = atan2f(mat.m[1][2], mat.m[2][2]);
		angles.y = atan2f(-mat.m[0][2], sqrtf(mat.m[1][2] * mat.m[2][2] * mat.m[2][2]));;
		angles.z = atan2f(mat.m[0][1], mat.m[0][0]);

		return angles;
	}

	inline void SetBillboard(const KdMatrix& mat)
	{

	}
};

inline KdMatrix operator* (const KdMatrix& M1, const KdMatrix& M2) 
{
	using namespace DirectX;
	return XMMatrixMultiply(M1, M2);
}