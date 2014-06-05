#pragma once

#include "BsPrerequisites.h"
#include "BsCoreThreadAccessor.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Handles overlay rendering by determining which overlay gets rendered 
	 *			on which render target. Overlays are always rendered on top of any geometry.
	 * 			
	 * @note	Overlays could have been stored directly on a RenderTarget but this class
	 * 			was created to decouple the connection.
	 * 
	 * @see Overlay
	 */
	class BS_EXPORT OverlayManager : public Module<OverlayManager>
	{
		/**
		 * @brief	Used for comparing overlays in order to determine their rendering order.
		 */
		struct OverlayComparer
		{
			bool operator() (const Overlay* const& a, const Overlay* const& b);
		};

	public:
		/**
		 * @brief	Schedules any overlays for the specified viewport to be rendered.
		 *			(adds them to the render queue).
		 */
		void render(ViewportPtr& target, DrawList& drawList) const;

	private:
		friend class Overlay;

		/**
		 * @brief	Attaches the specified overlay to the viewport.
		 */
		void attachOverlay(const Viewport* target, const Overlay* overlay);

		/**
		 * @brief	Detaches the specified overlay from the viewport.
		 */
		void detachOverlay(const Viewport* target, const Overlay* overlay);

		/**
		 * @brief	Detaches the specified overlay from all viewports.
		 */
		void detachOverlayFromAll(const Overlay* overlay);

		UnorderedMap<const Viewport*, Set<const Overlay*, OverlayComparer>> mOverlaysPerTarget;
	};
}