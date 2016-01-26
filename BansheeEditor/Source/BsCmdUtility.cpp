//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsCmdUtility.h"
#include "BsSceneObject.h"

namespace BansheeEngine
{
	CmdUtility::SceneObjProxy CmdUtility::createProxy(const HSceneObject& sceneObject)
	{
		struct TempData
		{
			TempData(SceneObjProxy& proxy, const HSceneObject& so)
				:proxy(proxy), obj(so)
			{ }

			SceneObjProxy& proxy;
			HSceneObject obj;
		};

		SceneObjProxy rootProxy;

		Stack<TempData> todo;
		todo.push(TempData(rootProxy, sceneObject));

		while (!todo.empty())
		{
			TempData data = todo.top();
			todo.pop();

			data.proxy.instanceData = data.obj->_getInstanceData();

			const Vector<HComponent>& components = data.obj->getComponents();
			for (auto& component : components)
				data.proxy.componentInstanceData.push_back(component->_getInstanceData());

			UINT32 numChildren = data.obj->getNumChildren();
			data.proxy.children.resize(numChildren);
			for (UINT32 i = 0; i < numChildren; i++)
			{
				todo.push(TempData(data.proxy.children[i], data.obj->getChild(i)));
			}
		}

		return rootProxy;
	}


	void CmdUtility::restoreIds(const HSceneObject& restored, SceneObjProxy& proxy)
	{
		// Note: This method relies that all restored GameObject handles pointing to the
		// same object also have the same shared handle data (Since I only update instance
		// data on a single handle I know exists, and expect all others will be updated
		// by that as well).

		struct TempData
		{
			TempData(SceneObjProxy& proxy, const HSceneObject& restoredObj)
			:proxy(proxy), restoredObj(restoredObj)
			{ }

			SceneObjProxy& proxy;
			HSceneObject restoredObj;
		};

		Stack<TempData> todo;
		todo.push(TempData(proxy, restored));

		while (!todo.empty())
		{
			TempData data = todo.top();
			todo.pop();

			data.restoredObj->_setInstanceData(data.proxy.instanceData);

			// Find components that are still active and swap the old ones with restored ones,
			// keep any other as is.
			const Vector<HComponent>& restoredComponents = data.restoredObj->getComponents();

			UINT32 idx = 0;
			for (auto& restoredComponent : restoredComponents)
			{
				restoredComponent->_setInstanceData(data.proxy.componentInstanceData[idx]);

				GameObjectPtr restoredPtr = std::static_pointer_cast<GameObject>(restoredComponent.getInternalPtr());
				HComponent restoredComponentCopy = restoredComponent; // To remove const
				restoredComponentCopy._setHandleData(restoredPtr);

				idx++;
			}
			
			// Find children that are still active and swap the old ones with restored ones,
			// keep any other as is
			UINT32 restoredNumChildren = data.restoredObj->getNumChildren();
			for (UINT32 i = 0; i < restoredNumChildren; i++)
			{
				todo.push(TempData(data.proxy.children[i], data.restoredObj->getChild(i)));
			}
		}
	}
}