#include "KdResouceFactory.h"

std::shared_ptr<KdModel> KdResouceFactory::GetModel(const std::string& filename)
{
	//filenameのモデルがあるか
	auto itFound = m_modelMap.find(filename);
	//ない場合
	if(itFound == m_modelMap.end())
	{
		//生成＆読み込み
		auto newModel = std::make_shared<KdModel>();
		if (newModel->Load(filename) == false)
		{
			//読み込み失敗時は、nullを返す
			return nullptr;
		}
		//リスト（map）に登録
		m_modelMap[filename] = newModel;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}
}

std::shared_ptr<KdTexture> KdResouceFactory::GetTexture(const std::string& filename)
{
	//filenameのテクスチャがあるか
	auto itFound = m_texMap.find(filename);

	//ない場合
	if (itFound == m_texMap.end())
	{
		//生成＆読み込み
		auto newTexture = std::make_shared<KdTexture>();
		if (newTexture->Load(filename) == false)
		{
			//読み込み失敗時は、nullを返す
			return nullptr;
		}
		//リスト（map）に登録
		m_texMap[filename] = newTexture;
		//リソースを返す
		return newTexture;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}

}

json11::Json KdResouceFactory::GetJSON(const std::string& filename)
{
	//filenameのモデルがあるか
	auto itFound = m_jsonMap.find(filename);
	//ない場合
	if (itFound == m_jsonMap.end())
	{
		//JSONファイルを文字列として読み込む
		json11::Json json = LoadJSON(filename);
		if (json.is_null())
		{
			assert(0 && "[GetJSON]jsonファイルが見つからない");
			return nullptr;
		}
		//リスト（map）に登録
		m_jsonMap[filename] = json;
	}
	//ある場合
	else
	{
		return (*itFound).second;
	}
}

//JSON読み込み
inline json11::Json KdResouceFactory::LoadJSON(const std::string& filename)
{
	//Jsonファイルを開く
	std::ifstream ifs(filename);
	if (ifs.fail())
	{
		assert(0 && "jsonファイルのパスが間違っています");
		return nullptr;
	}
	//文字列として全読み込み
	std::string strJson((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

	//文字列のJSONを解析する
	std::string err;

	json11::Json jsonObj = json11::Json::parse(strJson, err);

	if (err.size() > 0)
	{
		assert(0 && "読み込んだファイルのJSON変換に失敗");
		return nullptr;
	}

	return jsonObj;
}