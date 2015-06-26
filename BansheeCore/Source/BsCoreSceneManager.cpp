#include "BsCoreSceneManager.h"
#include "BsSceneObject.h"
#include "BsComponent.h"
#include "BsGameObjectManager.h"

namespace BansheeEngine
{
	std::function<void()> SceneManagerFactory::mFactoryMethod;

	CoreSceneManager::CoreSceneManager()
	{
		mRootNode = SceneObject::createInternal("SceneRoot");
	}

	CoreSceneManager::~CoreSceneManager()
	{
		if (mRootNode != nullptr && !mRootNode.isDestroyed())
			mRootNode->destroy(true);
	}

	void CoreSceneManager::clearScene()
	{
		GameObjectManager::instance().destroyQueuedObjects();
		mRootNode->destroy(true);
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

		GameObjectManager::instance().destroyQueuedObjects();
	}

	void CoreSceneManager::registerNewSO(const HSceneObject& node) 
	{ 
		if(mRootNode)
			node->setParent(mRootNode);
	}

	CoreSceneManager& gCoreSceneManager()
	{
		return CoreSceneManager::instance();
	}

	void SceneManagerFactory::create()
	{
		if (mFactoryMethod != nullptr)
			mFactoryMethod();
		else
			BS_EXCEPT(InvalidStateException, "Failed to initialize scene manager because no valid factory method is set.");
	}
}