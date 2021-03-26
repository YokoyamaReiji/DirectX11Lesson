#pragma once

class KdModel
{
public:
	KdModel();
	~KdModel();

	bool Load(const std::string& filename); 
	
	//アクセサ
	//const std::shared_ptr<KdMesh> GetMesh() const { return m_spMesh; }
	const std::shared_ptr<KdMesh> GetMesh(UINT index) const
	{
		return index < m_originalNodes.size() ? m_originalNodes[index].m_spMesh : nullptr;
	}

	const std::vector<KdMaterial>& GetMaterials() const { return m_materials; }

	//ノード：モデルを形成するメッシュを扱うための最小単位
	struct Node
	{
		std::string m_name;//ノード名
		KdMatrix m_localTransform;//変換行列(原点からどれだけ離れているか)
		std::shared_ptr<KdMesh>m_spMesh;//メッシュ情報
	};

	inline Node* FindNode(const std::string& name)
	{
		for (auto&& node : m_originalNodes)
		{
			if (node.m_name == name) { return &node; }
		}
		return nullptr;
	}

	//ノード取得配列
	const std::vector<Node>& GetOriginalNodes() const { return m_originalNodes; }
private:

	void Release();							 //解放
	std::vector<Node>       m_originalNodes; //データのノード配列

	//マテリアル配列
	std::vector<KdMaterial> m_materials;
};