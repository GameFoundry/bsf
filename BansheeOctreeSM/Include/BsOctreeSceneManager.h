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

		const Vector<HCamera>::type& getAllCameras() const { return mCachedCameras; }

		Vector<HRenderable>::type getVisibleRenderables(const HCamera& camera) const;

		void updateRenderableBounds();
	private:
		void notifyComponentAdded(const HComponent& component);
		void notifyComponentRemoved(const HComponent& component);

		Vector<HCamera>::type mCachedCameras;
		Vector<HRenderable>::type mRenderables;
	};
}