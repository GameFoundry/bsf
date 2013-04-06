#include "CmSceneManager.h"
#include "CmGameObject.h"
#include "CmComponent.h"
#include "CmCamera.h"
#include "CmRenderable.h"

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

	void SceneManager::update()
	{
		stack<HGameObject>::type todo;
		todo.push(mRootNode);

		while(!todo.empty())
		{
			HGameObject currentGO = todo.top();
			todo.pop();
			                  
			vector<ComponentPtr>::type components = currentGO->getComponents();

			for(auto iter = components.begin(); iter != components.end(); ++iter)
			{
				(*iter)->update();
			}

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}
	}

	vector<RenderablePtr>::type SceneManager::getVisibleRenderables(const CameraPtr camera) const
	{
		// TODO - Cull invisible objects

		vector<RenderablePtr>::type renderables;

		stack<HGameObject>::type todo;
		todo.push(mRootNode);

		while(!todo.empty())
		{
			HGameObject currentGO = todo.top();
			todo.pop();

			RenderablePtr curRenderable = currentGO->getComponent<Renderable>();
			if(curRenderable != nullptr)
				renderables.push_back(curRenderable);

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}

		return renderables;
	}

	void SceneManager::registerNewGO(const HGameObject& node) 
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