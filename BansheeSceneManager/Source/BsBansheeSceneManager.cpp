#include "BsBansheeSceneManager.h"
#include "BsComponent.h"
#include "BsException.h"
#include "BsSceneObject.h"
#include "BsRenderable.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	void BansheeSceneManager::updateRenderableTransforms()
	{
		// TODO - Consider a way to make the update faster. Either do it concurrently or
		// consider organizing renderable matrices in an array for quicker updates
		//   - I could keep everything in a sequential array but deal with dynamic elements
		//     but putting them in a slow, normal array. Once the number of dynamic elements
		//	   goes over some number the hierarchy is re-optimized.

		for(auto& iter : mRenderables)
		{
			iter->SO()->updateTransformsIfDirty();
		}
	}

	void BansheeSceneManager::notifyComponentAdded(const HComponent& component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			HCamera camera = static_object_cast<Camera>(component);
			auto findIter = std::find(mCachedCameras.begin(), mCachedCameras.end(), camera);

			if(findIter != mCachedCameras.end())
			{
				BS_EXCEPT(InternalErrorException, "Trying to add an already existing camera!");
			}

			mCachedCameras.push_back(camera);
		}
		else if(component->getTypeId() == TID_Renderable)
		{
			HRenderable renderable = static_object_cast<Renderable>(component);
			mRenderables.push_back(renderable);
		}
	}

	void BansheeSceneManager::notifyComponentRemoved(const HComponent& component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			HCamera camera = static_object_cast<Camera>(component);
			auto findIter = std::find(mCachedCameras.begin(), mCachedCameras.end(), camera);

			if(findIter == mCachedCameras.end())
			{
				BS_EXCEPT(InternalErrorException, "Cannot find specified camera!");
			}

			mCachedCameras.erase(findIter);

			onCameraRemoved(camera);
		}
		else if(component->getTypeId() == TID_Renderable)
		{
			HRenderable renderable = static_object_cast<Renderable>(component);

			// TODO - I should probably use some for of a hash set because searching through possibly thousands of renderables will be slow
			auto findIter = std::find(mRenderables.begin(), mRenderables.end(), renderable);
			if(findIter != mRenderables.end())
				mRenderables.erase(findIter);

			onRenderableRemoved(renderable);
		}
	}
}