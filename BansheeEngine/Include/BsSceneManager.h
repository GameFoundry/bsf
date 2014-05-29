#pragma once

#include "BsPrerequisites.h"
#include "BsCoreSceneManager.h"

namespace BansheeEngine
{
	class BS_EXPORT SceneManager : public CoreSceneManager
	{
	public:
		SceneManager() {}
		virtual ~SceneManager() {}

		/**
		 * @brief	Returns all cameras in the scene.
		 */
		virtual const Vector<HCamera>& getAllCameras() const = 0;

		/**
		 * @brief	Returns all renderables visible to the specified camera.
		 */
		virtual Vector<HRenderable> getVisibleRenderables(const HCamera& camera) const = 0;

		virtual void updateRenderableBounds() = 0;
	};

	BS_EXPORT SceneManager& gBsSceneManager();
}