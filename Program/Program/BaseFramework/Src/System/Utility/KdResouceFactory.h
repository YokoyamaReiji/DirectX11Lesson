#pragma once

//==================================
//リソース管理クラス
//・デザインパターンのFlyweightパターンを採用
//=====================================

class KdResouceFactory
{
public:
	//モデルデータ取得
	std::shared_ptr<KdModel> GetModel(const std::string& filename);

	//管理を破棄する
	void Clear()
	{
		m_modelMap.clear();
	}
private:
	//モデルデータ管理マップ
	std::unordered_map<std::string, std::shared_ptr<KdModel>> m_modelMap;

	//====================================
	//シングルトン
	//===================================
private:
	KdResouceFactory(){}
public:
	static KdResouceFactory& GetInstance()
	{
		static KdResouceFactory instance;
		return instance;
	}

};

#define KdResFac KdResouceFactory::GetInstance()