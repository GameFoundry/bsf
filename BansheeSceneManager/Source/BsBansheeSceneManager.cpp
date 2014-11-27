#include "BsBansheeSceneManager.h"
#include "BsComponent.h"
#include "BsException.h"
#include "BsSceneObject.h"
#include "BsRenderable.h"
#include "BsCamera.h"

namespace BansheeEngine
{
	void BansheeSceneManager::notifyComponentAdded(const HComponent& component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			HCamera camera = static_object_cast<Camera>(component);
			CameraHandlerPtr cameraHandler = camera->_getHandler();

			auto findIter = std::find_if(mCachedCameras.begin(), mCachedCameras.end(), 
				[&](const SceneCameraData& x) { return x.camera == cameraHandler; });

			if(findIter != mCachedCameras.end())
			{
				BS_EXCEPT(InternalErrorException, "Trying to add an already existing camera!");
			}

			mCachedCameras.push_back(SceneCameraData(cameraHandler, component->SO()));
		}
		else if(component->getTypeId() == TID_Renderable)
		{
			HRenderable renderable = static_object_cast<Renderable>(component);
			RenderableHandlerPtr renderableHandler = renderable->_getHandler();

			mRenderables.push_back(SceneRenderableData(renderableHandler, renderable->SO()));
		}
	}

	void BansheeSceneManager::notifyComponentRemoved(const HComponent& component)
	{
		if(component->getTypeId() == TID_Camera)
		{
			HCamera camera = static_object_cast<Camera>(component);
			CameraHandlerPtr cameraHandler = camera->_getHandler();

			auto findIter = std::find_if(mCachedCameras.begin(), mCachedCameras.end(),
				[&](const SceneCameraData& x) { return x.camera == cameraHandler; });

			if(findIter == mCachedCameras.end())
			{
				BS_EXCEPT(InternalErrorException, "Cannot find specified camera!");
			}

			mCachedCameras.erase(findIter);
		}
		else if(component->getTypeId() == TID_Renderable)
		{
			HRenderable renderable = static_object_cast<Renderable>(component);
			RenderableHandlerPtr renderableHandler = renderable->_getHandler();

			// TODO - I should probably use some for of a hash set because searching through possibly thousands of renderables will be slow
			auto findIter = std::find_if(mRenderables.begin(), mRenderables.end(),
				[&](const SceneRenderableData& x) { return x.renderable == renderableHandler; });

			if(findIter != mRenderables.end())
				mRenderables.erase(findIter);

			onRenderableRemoved(renderableHandler);
		}
	}
}