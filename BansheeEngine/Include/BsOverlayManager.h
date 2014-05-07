#pragma once

#include "BsPrerequisites.h"
#include "CmCoreThreadAccessor.h"
#include "CmModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Takes care which overlay gets rendered on which render target.
	 * 			
	 * @note	Overlays could have been stored directly on a RenderTarget but this class
	 * 			was created to decouple the connection.
	 * 
	 * @see Overlay
	 */
	class BS_EXPORT OverlayManager : public Module<OverlayManager>
	{
		struct OverlayComparer
		{
			bool operator() (const Overlay* const& a, const Overlay* const& b);
		};

	public:
		void render(ViewportPtr& target, RenderQueue& renderQueue) const;

	private:
		friend class Overlay;

		void attachOverlay(const Viewport* target, const Overlay* overlay);
		void detachOverlay(const Viewport* target, const Overlay* overlay);
		void detachOverlayFromAll(const Overlay* overlay);

		UnorderedMap<const Viewport*, Set<const Overlay*, OverlayComparer>> mOverlaysPerTarget;
	};
}