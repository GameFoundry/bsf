#include "CmSceneManager.h"
#include "CmSceneObject.h"
#include "CmComponent.h"

namespace BansheeEngine
{
	SceneManagerBase::SceneManagerBase()
	{
		mRootNode = SceneObject::createInternal("SceneRoot");
	}

	SceneManagerBase::~SceneManagerBase()
	{
		if(mRootNode != nullptr)
			mRootNode->destroy();
	}

	void SceneManagerBase::update()
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

	void SceneManagerBase::registerNewGO(const HSceneObject& node) 
	{ 
		if(mRootNode) // If root node is null, then this new node is the root node
			node->setParent(mRootNode);
	}

	void SceneManagerBase::notifyComponentAdded(const HComponent& component) { }
	void SceneManagerBase::notifyComponentRemoved(const HComponent& component) { }

	SceneManagerBase& gSceneManager()
	{
		return SceneManagerBase::instance();
	}
}