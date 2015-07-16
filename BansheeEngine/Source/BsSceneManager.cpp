#include "BsSceneManager.h"
#include "BsSceneObject.h"
#include "BsRenderableHandler.h"
#include "BsCameraHandler.h"

namespace BansheeEngine
{
	volatile SceneManager::InitOnStart SceneManager::DoInitOnStart;

	void SceneManager::_registerRenderable(const SPtr<RenderableHandler>& renderable, const HSceneObject& so)
	{
		mRenderables[renderable.get()] = SceneRenderableData(renderable, so);
	}

	void SceneManager::_unregisterRenderable(const SPtr<RenderableHandler>& renderable)
	{
		mRenderables.erase(renderable.get());
	}

	void SceneManager::_registerCamera(const SPtr<CameraHandler>& camera, const HSceneObject& so)
	{
		mCameras[camera.get()] = SceneCameraData(camera, so);
	}

	void SceneManager::_unregisterCamera(const SPtr<CameraHandler>& camera)
	{
		mCameras.erase(camera.get());
	}

	void SceneManager::_updateCoreObjectTransforms()
	{
		for (auto& renderablePair : mRenderables)
		{
			RenderableHandlerPtr handler = renderablePair.second.renderable;
			HSceneObject so = renderablePair.second.sceneObject;

			UINT32 curHash = so->getTransformHash();
			if (curHash != handler->_getLastModifiedHash())
			{
				handler->setTransform(so->getWorldTfrm());
				handler->_setLastModifiedHash(curHash);
			}

			if (so->getActive() != handler->getIsActive())
			{
				handler->setIsActive(so->getActive());
			}
		}

		for (auto& cameraPair : mCameras)
		{
			CameraHandlerPtr handler = cameraPair.second.camera;
			HSceneObject so = cameraPair.second.sceneObject;

			UINT32 curHash = so->getTransformHash();
			if (curHash != handler->_getLastModifiedHash())
			{
				handler->setPosition(so->getWorldPosition());
				handler->setRotation(so->getWorldRotation());

				handler->_setLastModifiedHash(curHash);
			}
		}
	}

	SceneManager& SceneManager::instance()
	{
		return static_cast<SceneManager&>(CoreSceneManager::instance());
	}

	SceneManager* SceneManager::instancePtr()
	{
		return static_cast<SceneManager*>(CoreSceneManager::instancePtr());
	}

	SceneManager& gSceneManager()
	{
		return static_cast<SceneManager&>(gCoreSceneManager());
	}
}
