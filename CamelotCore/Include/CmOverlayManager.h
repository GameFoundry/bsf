#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Takes care which overlay gets rendered on which camera.
	 * 			
	 * @note	Overlays could have been stored directly on a Camera but this class
	 * 			was created to decouple the connection.
	 * 
	 * @see Overlay
	 */
	class CM_EXPORT OverlayManager : public Module<OverlayManager>
	{
	public:
		void render(const Camera* camera, DeferredRenderContextPtr& renderContext) const;

		void attachOverlay(const Camera* camera, const Overlay* overlay);
		void detachOverlay(const Camera* camera, const Overlay* overlay);
		void detachOverlayFromAll(const Overlay* overlay);
	private:
		unordered_map<const Camera*, unordered_set<const Overlay*>::type>::type mOverlaysPerCamera;
	};
}