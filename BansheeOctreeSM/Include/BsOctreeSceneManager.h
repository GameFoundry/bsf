#pragma once

#include "BsOctreeSMPrerequisites.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Default scene manager implementation. Allows you to query
	 *			the scene graph for various uses.
	 *
	 * TODO - Update doc when I implement this properly
	 */
	class BS_SM_EXPORT OctreeSceneManager : public SceneManager
	{
	public:
		OctreeSceneManager() {}
		~OctreeSceneManager() {}

		/**
		 * @brief	Returns a list of all cameras in the scene.
		 */
		const Vector<HCamera>& getAllCameras() const { return mCachedCameras; }

		/**
		 * @brief	Returns a list of all renderables visible by the provided camera.
		 */
		Vector<HRenderable> getVisibleRenderables(const HCamera& camera) const;

		/**
		 * @brief	Update world bounds of all renderables. This should be called
		 *			at the end or beginning of every frame to ensure bounds are kept
		 *			up to date.
		 */
		void updateRenderableBounds();
	private:
		/**
		 * @brief	Called by scene objects whenever a new component is added to the scene.
		 */
		void notifyComponentAdded(const HComponent& component);

		/**
		 * @brief	Called by scene objects whenever a new component is destroyed.
		 */
		void notifyComponentRemoved(const HComponent& component);

		Vector<HCamera> mCachedCameras;
		Vector<HRenderable> mRenderables;
	};
}