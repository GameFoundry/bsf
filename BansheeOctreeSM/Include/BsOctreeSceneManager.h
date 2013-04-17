#pragma once

#include "BsOctreeSMPrerequisites.h"
#include "BsSceneManager.h"

namespace BansheeEngine
{
	class BS_SM_EXPORT OctreeSceneManager : public SceneManager
	{
	public:
		OctreeSceneManager() {}
		~OctreeSceneManager() {}

		const std::vector<HCamera>& getAllCameras() const { return mCachedCameras; }

		std::vector<HRenderable> getVisibleRenderables(const HCamera& camera) const;
	private:
		void notifyComponentAdded(const CM::HComponent& component);
		void notifyComponentRemoved(const CM::HComponent& component);

		std::vector<HCamera> mCachedCameras;
	};
}