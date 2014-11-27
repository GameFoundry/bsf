#pragma once

#include "BsBansheeSMPrerequisites.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Default scene manager implementation. Allows you to query
	 *			the scene graph for various uses.
	 *
	 * TODO - Update doc when I implement this properly
	 */
	class BS_SM_EXPORT BansheeSceneManager : public SceneManager
	{
	public:
		BansheeSceneManager() {}
		~BansheeSceneManager() {}

		/**
		 * @copydoc	SceneManager::getAllCameras
		 */
		const Vector<SceneCameraData>& getAllCameras() const { return mCachedCameras; }

		/**
		 * @copydoc	SceneManager::getAllRenderables
		 */
		const Vector<SceneRenderableData>& getAllRenderables() const { return mRenderables; }

	private:
		/**
		 * @brief	Called by scene objects whenever a new component is added to the scene.
		 */
		void notifyComponentAdded(const HComponent& component);

		/**
		 * @brief	Called by scene objects whenever a new component is destroyed.
		 */
		void notifyComponentRemoved(const HComponent& component);

		Vector<SceneCameraData> mCachedCameras;
		Vector<SceneRenderableData> mRenderables;
	};
}