#include "BsOctreeSceneManager.h"
#include "CmComponent.h"
#include "CmException.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"
#include "BsCamera.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Vector<HRenderable>::type OctreeSceneManager::getVisibleRenderables(const HCamera& camera) const
	{
		// TODO - Cull invisible objects

		Vector<HRenderable>::type renderables;

		Stack<HSceneObject>::type todo;
		todo.push(mRootNode);

		while(!todo.empty())
		{
			HSceneObject currentGO = todo.top();
			todo.pop();

			HRenderable curRenderable = currentGO->getComponent<Renderable>();
			if(curRenderable != nullptr)
				renderables.push_back(curRenderable);

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}

		return renderables;
	}

	void OctreeSceneManager::notifyComponentAdded(const HComponent& component)
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

	void OctreeSceneManager::notifyComponentRemoved(const HComponent& component)
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
}