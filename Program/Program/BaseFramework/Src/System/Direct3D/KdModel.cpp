#include "KdModel.h"
#include "KdGLTFLoader.h"

//コンストラクタ
KdModel::KdModel()
{

}

//デストラクタ
KdModel::~KdModel() 
{
	Release();
}

//解放
void KdModel::Release()
{
	m_materials.clear();
	m_originalNodes.clear();
}

//ロード関数
/*bool KdModel::Load(const std::string& filename) 
{
	std::string fileDir = KdGetDirFromPath(filename);

	auto spModel = KdLoadGLTFModel(filename);
	if (spModel == nullptr) 
	{
		return false;
	}

	//メッシュの受け取り
	for (UINT i = 0; i < spModel->Nodes.size(); ++i) 
	{
		const KdGLTFNode& rNode = spModel->Nodes[i];

		if (rNode.IsMesh) 
		{
			m_spMesh = std::make_shared<KdMesh>();

			if (m_spMesh) 
			{
				//						頂点情報配列		面情報配列		サブセット情報配列
				m_spMesh->Create(rNode.Mesh.Vertices, rNode.Mesh.Faces, rNode.Mesh.Subsets);
				break;
			}
		}
	}

	//マテリアル配列を受け取れるサイズのメモリを確保
	m_materials.resize(spModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); ++i)
	{
		//src=Sourceの略
		//dst=destinationの略
		const KdGLTFMaterial& rSrcMaterual = spModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		//名前
		rDstMaterial.Name = rSrcMaterual.Name;

		//基本色
		rDstMaterial.BaseColor = rSrcMaterual.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();

		//テクスチャ読み込み
		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterual.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}
	}
	return true;
}*/

bool KdModel::Load(const std::string& filename)
{
	//ファイルの完全パスを取得
	std::string fileDir = KdGetDirFromPath(filename);

	//GLTFの読み込み
	std::shared_ptr<KdGLTFModel>spGltfModel = KdLoadGLTFModel(filename);
	if (spGltfModel == nullptr) { return false; }
    
	//ノード格納場所のメモリ確保
	m_originalNodes.resize(spGltfModel->Nodes.size());

	//メッシュの受け取り
	for (UINT i = 0; i < spGltfModel->Nodes.size(); i++)
	{
		//入力元ノード
		const KdGLTFNode& rSrcNode = spGltfModel->Nodes[i];

		//出力先ノード
		Node& rDstNode = m_originalNodes[i];

		//ノード情報のセット
		rDstNode.m_name = rSrcNode.Name;
		rDstNode.m_localTransform = rSrcNode.LocalTransform;

		//ノードの内容がメッシュだったら
		if (rSrcNode.IsMesh)
		{
			//リストにメッシュのインスタンス化
			rDstNode.m_spMesh = std::make_shared<KdMesh>();

			//メッシュ情報の作成
			if (rDstNode.m_spMesh)
			{
				rDstNode.m_spMesh->Create(
					rSrcNode.Mesh.Vertices, rSrcNode.Mesh.Faces, rSrcNode.Mesh.Subsets);
			}
		}
	}
	//マテリアル配列を受け取れるサイズのメモリを確保
	m_materials.resize(spGltfModel->Materials.size());

	for (UINT i = 0; i < m_materials.size(); ++i)
	{
		//src=Sourceの略
		//dst=destinationの略
		const KdGLTFMaterial& rSrcMaterual = spGltfModel->Materials[i];
		KdMaterial& rDstMaterial = m_materials[i];

		//名前
		rDstMaterial.Name = rSrcMaterual.Name;

		//基本色
		rDstMaterial.BaseColor = rSrcMaterual.BaseColor;
		rDstMaterial.BaseColorTex = std::make_shared<KdTexture>();

		//テクスチャ読み込み
		rDstMaterial.BaseColorTex = KdResFac.GetTexture(fileDir + rSrcMaterual.BaseColorTexture);
		if (rDstMaterial.BaseColorTex == nullptr)
		{
			rDstMaterial.BaseColorTex = D3D.GetWhiteTex();
		}
	}
	return true;
}