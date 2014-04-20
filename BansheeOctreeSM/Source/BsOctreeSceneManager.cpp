#include "BsOctreeSceneManager.h"
#include "CmComponent.h"
#include "CmException.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"
#include "BsCamera.h"

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
			{
				if((curRenderable->getLayer() & camera->getLayers()) != 0)
					renderables.push_back(curRenderable);
			}

			for(UINT32 i = 0; i < currentGO->getNumChildren(); i++)
				todo.push(currentGO->getChild(i));
		}

		return renderables;
	}

	void OctreeSceneManager::updateRenderableBounds()
	{
		for(auto& iter : mRenderables)
		{
			iter->updateWorldBounds();
		}
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
		else if(component->getTypeId() == TID_Renderable)
		{
			HRenderable renderable = static_object_cast<Renderable>(component);
			mRenderables.push_back(renderable);
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
		else if(component->getTypeId() == TID_Renderable)
		{
			HRenderable renderable = static_object_cast<Renderable>(component);

			// TODO - I should probably use some for of a hash set because searching through possibly thousands of renderables will be slow
			auto findIter = std::find(mRenderables.begin(), mRenderables.end(), renderable);
			if(findIter != mRenderables.end())
				mRenderables.erase(findIter);
		}
	}
}