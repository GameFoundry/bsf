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
			                  
			vector<HComponent>::type components = currentGO->getComponents();

			for(auto iter = components.begin(); iter != components.end(); ++iter)
			{
				(*iter)->update();
			}

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}
	}

	vector<HRenderable>::type SceneManager::getVisibleRenderables(const HCamera& camera) const
	{
		// TODO - Cull invisible objects

		vector<HRenderable>::type renderables;

		stack<HGameObject>::type todo;
		todo.push(mRootNode);

		while(!todo.empty())
		{
			HGameObject currentGO = todo.top();
			todo.pop();

			HRenderable curRenderable = currentGO->getComponent<Renderable>();
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

	void SceneManager::notifyComponentAdded(const HComponent& component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			HCamera camera = static_object_cast<Camera>(component);
			auto findIter = std::find(mCachedCameras.begin(), mCachedCameras.end(), camera);

			if(findIter != mCachedCameras.end())
			{
				CM_EXCEPT(InternalErrorException, "Trying to add an already existing camera!");
			}

			mCachedCameras.push_back(camera);
		}
	}

	void SceneManager::notifyComponentRemoved(const HComponent& component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			HCamera camera = static_object_cast<Camera>(component);
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