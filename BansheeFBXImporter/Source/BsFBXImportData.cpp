#include "BsFBXImportData.h"

namespace BansheeEngine
{
	FBXImportNode::~FBXImportNode()
	{
		for (auto& child : children)
			bs_delete(child);
	}

	FBXImportScene::FBXImportScene()
		:rootNode(nullptr)
	{ }

	FBXImportScene::~FBXImportScene()
	{
		if (rootNode != nullptr)
			bs_delete(rootNode);

		for (auto& mesh : meshes)
			bs_delete(mesh);
	}
}