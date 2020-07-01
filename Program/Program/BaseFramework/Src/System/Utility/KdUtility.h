﻿#pragma once

//===========================================
//
// 便利機能
//
//===========================================

// 算術系短縮名
namespace Math = DirectX::SimpleMath;

// 角度変換
constexpr float KdToRadians = (3.141592654f / 180.0f);
constexpr float KdToDegrees = (180.0f / 3.141592654f);

// 安全にReleaseするための関数
template<class T>
void KdSafeRelease(T*& p)
{
	if (p)
	{
		p->Release();
		p = nullptr;
	}
}

// 安全にDeleteするための関数
template<class T>
void KdSafeDelete(T*& p)
{
	if (p)
	{
		delete p;
		p = nullptr;
	}
}

//===========================================
//
// 色定数
//
//===========================================
static const Math::Color	kWhiteColor		= Math::Color(1, 1, 1, 1);
static const Math::Color	kBlackColor		= Math::Color(0, 0, 0, 1);
static const Math::Color	kRedColor		= Math::Color(1, 0, 0, 1);
static const Math::Color	kGreenColor		= Math::Color(0, 1, 0, 1);
static const Math::Color	kBlueColor		= Math::Color(0, 0, 1, 1);


//===========================================
//
// ファイル
//
//===========================================

// ファイルパスから、親ディレクトリまでのパスを取得
inline std::string KdGetDirFromPath(const std::string &path)
{
	const std::string::size_type pos = std::max<signed>(path.find_last_of('/'), path.find_last_of('\\'));
	return (pos == std::string::npos) ? std::string() : path.substr(0, pos + 1);
}

//===========================================
//
// 文字列関係
//
//===========================================

// std::string版 sprintf
template <typename ... Args>
std::string KdFormat(const std::string& fmt, Args ... args)
{
	size_t len = std::snprintf(nullptr, 0, fmt.c_str(), args ...);
	std::vector<char> buf(len + 1);
	std::snprintf(&buf[0], len + 1, fmt.c_str(), args ...);
	return std::string(&buf[0], &buf[0] + len);
}

//=====================================================
//
// テクスチャ
//
//=====================================================

// viewから画像情報を取得する
void KdGetTextureInfo(ID3D11View* view, D3D11_TEXTURE2D_DESC& outDesc);

//=====================================================
//
// Json
//
//=====================================================

//Json読み込み
inline json11::Json KdLoadJson(const std::string& filename)
{
	//jsonファイルを開く
	std::ifstream ifs(filename);
	if (ifs.fill())
	{	
		assert(0 && "jsonのファイルのパスが間違っています"); 
		return nullptr;
	}

	//文字列として全読み込み
	std::string strjson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	//文字列もjsonを解析する
	std::string err;
	json11::Json jsonObj = json11::Json::parse(strjson, err);
	if (err.size() > 0)
	{
		assert(0 && "読み込んだファイルのjson変換に失敗"); 
		return nullptr;
	}

	return jsonObj;
}
