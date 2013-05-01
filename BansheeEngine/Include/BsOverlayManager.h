#pragma once

#include "BsPrerequisites.h"
#include "CmDeferredRenderContext.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Takes care which overlay gets rendered on which camera.
	 * 			
	 * @note	Overlays could have been stored directly on a Camera but this class
	 * 			was created to decouple the connection.
	 * 
	 * @see Overlay
	 */
	class BS_EXPORT OverlayManager : public CM::Module<OverlayManager>
	{
	public:
		void render(const Camera* camera, CM::RenderContext& renderContext) const;

		void attachOverlay(const Camera* camera, const Overlay* overlay);
		void detachOverlay(const Camera* camera, const Overlay* overlay);
		void detachOverlayFromAll(const Overlay* overlay);
	private:
		std::unordered_map<const Camera*, std::unordered_set<const Overlay*>> mOverlaysPerCamera;
	};
}