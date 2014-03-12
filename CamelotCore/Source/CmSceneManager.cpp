#include "CmSceneManager.h"
#include "CmSceneObject.h"
#include "CmComponent.h"

namespace CamelotFramework
{
	SceneManager::SceneManager()
	{
		mRootNode = SceneObject::createInternal("SceneRoot");
	}

	SceneManager::~SceneManager()
	{
		if(mRootNode != nullptr)
			mRootNode->destroy();
	}

	void SceneManager::update()
	{
		Stack<HSceneObject>::type todo;
		todo.push(mRootNode);

		while(!todo.empty())
		{
			HSceneObject currentGO = todo.top();
			todo.pop();
			                  
			const Vector<HComponent>::type& components = currentGO->getComponents();

			for(auto iter = components.begin(); iter != components.end(); ++iter)
			{
				(*iter)->update();
			}

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}
	}

	void SceneManager::registerNewGO(const HSceneObject& node) 
	{ 
		if(mRootNode) // If root node is null, then this new node is the root node
			node->setParent(mRootNode);
	}

	void SceneManager::notifyComponentAdded(const HComponent& component) { }
	void SceneManager::notifyComponentRemoved(const HComponent& component) { }

	SceneManager& gSceneManager()
	{
		return SceneManager::instance();
	}
}