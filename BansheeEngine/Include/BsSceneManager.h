//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsCoreSceneManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains information about a camera managed by the scene manager.
	 */
	struct SceneCameraData
	{
		SceneCameraData() { }

		SceneCameraData(const CameraPtr& camera, const HSceneObject& sceneObject)
			:camera(camera), sceneObject(sceneObject)
		{ }

		CameraPtr camera;
		HSceneObject sceneObject;
	};

	/**
	 * @brief	Contains information about a renderable managed by the scene manager.
	 */
	struct SceneRenderableData
	{
		SceneRenderableData() { }

		SceneRenderableData(const RenderablePtr& renderable, const HSceneObject& sceneObject)
			:renderable(renderable), sceneObject(sceneObject)
		{ }

		RenderablePtr renderable;
		HSceneObject sceneObject;
	};

	/**
	 * @brief	Contains information about a light managed by the scene manager.
	 */
	struct SceneLightData
	{
		SceneLightData() { }

		SceneLightData(const SPtr<Light>& light, const HSceneObject& sceneObject)
			:light(light), sceneObject(sceneObject)
		{ }

		SPtr<Light> light;
		HSceneObject sceneObject;
	};

	/**
	 * @brief	Manages active SceneObjects and provides ways for querying
	 *			and updating them or their components.
	 */
	class BS_EXPORT SceneManager : public CoreSceneManager
	{
	public:
		struct InitOnStart
		{
		public:
			InitOnStart()
			{
				SceneManagerFactory::setFactoryMethod(&startUp);
			}

			static void startUp()
			{
				CoreSceneManager::startUp<SceneManager>();
			}
		};

		SceneManager() { }
		virtual ~SceneManager() {}

		/**
		 * @brief	Returns all cameras in the scene.
		 *
		 * @note	Internal method.
		 */
		const Map<Camera*, SceneCameraData>& getAllCameras() const { return mCameras; }

		/**
		 * @brief	Returns all renderables in the scene.
		 *
		 * @note	Internal method.
		 */
		const Map<Renderable*, SceneRenderableData>& getAllRenderables() const { return mRenderables; }

		/**
		 * @brief	Returns the camera in the scene marked as main. Main camera controls the final render
		 *			surface that is displayed to the user. If there are multiple main cameras, the first one found
		 *			returned.
		 *
		 * @note	Internal method.
		 */
		SceneCameraData getMainCamera() const;

		/**
		 * @brief	Sets the render target that the main camera in the scene (if any) will render its view to. This
		 * 			generally means the main game window when running standalone, or the Game viewport when running
		 * 			in editor.
		 *
		 * @note	Internal method.
		 */
		void setMainRenderTarget(const RenderTargetPtr& rt);

		/**
		 * @brief	Notifies the scene manager that a new renderable was created.
		 * 
		 * @note	Internal method.
		 */
		void _registerRenderable(const SPtr<Renderable>& renderable, const HSceneObject& so);

		/**
		 * @brief	Notifies the scene manager that a renderable was removed.
		 *
		 * @note	Internal method.
		 */
		void _unregisterRenderable(const SPtr<Renderable>& renderable);

		/**
		 * @brief	Notifies the scene manager that a new camera was created.
		 *
		 * @note	Internal method.
		 */
		void _registerCamera(const SPtr<Camera>& camera, const HSceneObject& so);

		/**
		 * @brief	Notifies the scene manager that a camera was removed.
		 *
		 * @note	Internal method.
		 */
		void _unregisterCamera(const SPtr<Camera>& camera);

		/**
		 * @brief	Notifies the scene manager that a camera either became the main camera, or has stopped being main camera.
		 *
		 * @note	Internal method.
		 */
		void _notifyMainCameraStateChanged(const SPtr<Camera>& camera);

		/**
		 * @brief	Notifies the scene manager that a new light was created.
		 *
		 * @note	Internal method.
		 */
		void _registerLight(const SPtr<Light>& light, const HSceneObject& so);

		/**
		 * @brief	Notifies the scene manager that a light was removed.
		 *
		 * @note	Internal method.
		 */
		void _unregisterLight(const SPtr<Light>& light);

		/**
		 * @copydoc	CoreSceneManager::_updateCoreObjectTransforms
		 */
		virtual void _updateCoreObjectTransforms() override;

		/**
		 * @copydoc	CoreSceneManager::instance
		 */
		static SceneManager& instance();

		/**
		 * @copydoc	CoreSceneManager::instancePtr
		 */
		static SceneManager* instancePtr();

	private:
		/**
		 * @brief	Callback that is triggered when the main render target size is changed.
		 */
		void onMainRenderTargetResized();

		Map<Camera*, SceneCameraData> mCameras;
		Map<Renderable*, SceneRenderableData> mRenderables;
		Map<Light*, SceneLightData> mLights;
		Vector<SceneCameraData> mMainCameras;
		RenderTargetPtr mMainRT;

		HEvent mMainRTResizedConn;

		volatile static InitOnStart DoInitOnStart;
	};

	/**
	 * @copydoc	SceneManager
	 */
	BS_EXPORT SceneManager& gSceneManager();
}