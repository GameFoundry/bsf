#include "BsCoreSceneManager.h"
#include "BsSceneObject.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	CoreSceneManager::CoreSceneManager()
	{
		mRootNode = SceneObject::createInternal("SceneRoot");
	}

	CoreSceneManager::~CoreSceneManager()
	{
		if(mRootNode != nullptr)
			mRootNode->destroy();
	}

	void CoreSceneManager::_update()
	{
		Stack<HSceneObject> todo;
		todo.push(mRootNode);

		while(!todo.empty())
		{
			HSceneObject currentGO = todo.top();
			todo.pop();
			                  
			const Vector<HComponent>& components = currentGO->getComponents();

			for(auto iter = components.begin(); iter != components.end(); ++iter)
			{
				(*iter)->update();
			}

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}
	}

	void CoreSceneManager::registerNewSO(const HSceneObject& node) 
	{ 
		if(mRootNode) // If root node is null, then this new node is the root node
			node->setParent(mRootNode);
	}

	void CoreSceneManager::notifyComponentAdded(const HComponent& component) { }
	void CoreSceneManager::notifyComponentRemoved(const HComponent& component) { }

	CoreSceneManager& gSceneManager()
	{
		return CoreSceneManager::instance();
	}
}