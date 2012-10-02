#include "CmSceneManager.h"
#include "CmGameObject.h"

namespace CamelotEngine
{
	SceneManager::SceneManager()
	{
		mRootNode = GameObject::createInternal("SceneRoot");
	}

	SceneManager::~SceneManager()
	{
		if(mRootNode != nullptr)
			mRootNode->destroy();
	}

	void SceneManager::registerNewGO(GameObjectPtr node) 
	{ 
		if(mRootNode) // If root node is null, then this new node is the root node
			node->setParent(mRootNode);
	}

	SceneManager& gSceneManager()
	{
		return SceneManager::instance();
	}
}