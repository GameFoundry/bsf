#include "CmSceneManager.h"
#include "CmGameObject.h"
#include "CmComponent.h"
#include "CmCamera.h"

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

	vector<RenderablePtr>::type SceneManager::getVisibleRenderables(const CameraPtr camera) const
	{
		// TODO - Actually iterate over all game objects and find visible renderables

		return vector<RenderablePtr>::type();
	}

	void SceneManager::registerNewGO(GameObjectPtr node) 
	{ 
		if(mRootNode) // If root node is null, then this new node is the root node
			node->setParent(mRootNode);
	}

	void SceneManager::notifyComponentAdded(ComponentPtr component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			CameraPtr camera = std::static_pointer_cast<Camera>(component);
			auto findIter = std::find(mCachedCameras.begin(), mCachedCameras.end(), camera);

			if(findIter != mCachedCameras.end())
			{
				CM_EXCEPT(InternalErrorException, "Trying to add an already existing camera!");
			}

			mCachedCameras.push_back(camera);
		}
	}

	void SceneManager::notifyComponentRemoved(ComponentPtr component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			CameraPtr camera = std::static_pointer_cast<Camera>(component);
			auto findIter = std::find(mCachedCameras.begin(), mCachedCameras.end(), camera);

			if(findIter == mCachedCameras.end())
			{
				CM_EXCEPT(InternalErrorException, "Cannot find specified camera!");
			}

			mCachedCameras.erase(findIter);
		}
	}

	SceneManager& gSceneManager()
	{
		return SceneManager::instance();
	}
}