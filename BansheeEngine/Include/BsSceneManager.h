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

		SceneCameraData(const CameraHandlerPtr& camera, const HSceneObject& sceneObject)
			:camera(camera), sceneObject(sceneObject)
		{ }

		CameraHandlerPtr camera;
		HSceneObject sceneObject;
	};

	/**
	 * @brief	Contains information about a renderable managed by the scene manager.
	 */
	struct SceneRenderableData
	{
		SceneRenderableData() { }

		SceneRenderableData(const RenderableHandlerPtr& renderable, const HSceneObject& sceneObject)
			:renderable(renderable), sceneObject(sceneObject)
		{ }

		RenderableHandlerPtr renderable;
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
		const Map<CameraHandler*, SceneCameraData>& getAllCameras() const { return mCameras; }

		/**
		 * @brief	Returns all renderables in the scene.
		 *
		 * @note	Internal method.
		 */
		const Map<RenderableHandler*, SceneRenderableData>& getAllRenderables() const { return mRenderables; }

		/**
		 * @brief	Notifies the scene manager that a new renderable was created.
		 * 
		 * @note	Internal method.
		 */
		void _registerRenderable(const SPtr<RenderableHandler>& renderable, const HSceneObject& so);

		/**
		 * @brief	Notifies the scene manager that a renderable was removed.
		 *
		 * @note	Internal method.
		 */
		void _unregisterRenderable(const SPtr<RenderableHandler>& renderable);

		/**
		 * @brief	Notifies the scene manager that a new camera was created.
		 *
		 * @note	Internal method.
		 */
		void _registerCamera(const SPtr<CameraHandler>& camera, const HSceneObject& so);

		/**
		 * @brief	Notifies the scene manager that a camera was removed.
		 *
		 * @note	Internal method.
		 */
		void _unregisterCamera(const SPtr<CameraHandler>& camera);

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
		Map<CameraHandler*, SceneCameraData> mCameras;
		Map<RenderableHandler*, SceneRenderableData> mRenderables;

		volatile static InitOnStart DoInitOnStart;
	};

	/**
	 * @copydoc	SceneManager
	 */
	BS_EXPORT SceneManager& gSceneManager();
}