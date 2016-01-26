#pragma once

#include "BsPrerequisites.h"
#include "BsCoreThreadAccessor.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Overlay components are a special type of components that can be attached directly
	 * 			to a Viewport and used for rendering 2D graphics. Any overlay components will be 
	 *			rendered after any other scene objects, so these components are usually used for 
	 *			GUI elements and full screen effects.
	 */
	class BS_EXPORT Overlay : public Component
	{
	public:
		virtual ~Overlay();

		/**
		 * @brief	Called by the renderer when rendering is in progress. Allows the overlay
		 *			to queue itself for rendering.
		 */
		virtual void render(DrawList& drawList) const = 0;

		/**
		 * @copydoc	Component::update
		 */
		virtual void update() {}

		/**
		 * @brief	Returns the viewport onto which this overlay will be overlaid on.
		 */
		Viewport* getTarget() const { return mRenderTarget; }

		/**
		 * @brief	Depth determines in which order the overlays render in case they overlap.
		 * 			Smaller depth means the overlay is in front of anything with larger depth.
		 */
		INT32 getDepth() const { return mDepth; }

		/**
		 * @brief	Depth determines in which order the overlays render in case they overlap.
		 * 			Smaller depth means the overlay is in front of anything with larger depth.
		 */
		void setDepth(INT32 depth) { mDepth = depth; }

	protected:
		friend class SceneObject;

		Viewport* mRenderTarget;
		INT32 mDepth;

		Overlay(const HSceneObject& parent, Viewport* target);
	};
}