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
		SceneManager() {}
		virtual ~SceneManager() {}

		/**
		 * @brief	Returns all cameras in the scene.
		 */
		virtual const Vector<SceneCameraData>& getAllCameras() const = 0;

		/**
		 * @brief	Returns all renderables in the scene.
		 */
		virtual const Vector<SceneRenderableData>& getAllRenderables() const = 0;

		/**
		 * @copydoc	CoreSceneManager::instance
		 */
		static SceneManager& instance();

		/**
		 * @copydoc	CoreSceneManager::instancePtr
		 */
		static SceneManager* instancePtr();

		/**
		 * @brief	Triggered whenever a renderable is removed from a SceneObject.
		 */
		Event<void(const RenderableHandlerPtr&)> onRenderableRemoved;
	};

	/**
	 * @copydoc	SceneManager
	 */
	BS_EXPORT SceneManager& gSceneManager();
}