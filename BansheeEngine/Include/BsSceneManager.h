#pragma once

#include "BsPrerequisites.h"
#include "CmSceneManager.h"

namespace BansheeEngine
{
	class BS_EXPORT SceneManager : public CM::SceneManager
	{
	public:
		SceneManager() {}
		virtual ~SceneManager() {}

		/**
		 * @brief	Returns all cameras in the scene.
		 */
		virtual const std::vector<CM::HCamera>& getAllCameras() const = 0;

		/**
		 * @brief	Returns all renderables visible to the specified camera.
		 */
		virtual std::vector<CM::HRenderable> getVisibleRenderables(const CM::HCamera& camera) const = 0;
	};

	BS_EXPORT SceneManager& gSceneManager();
}