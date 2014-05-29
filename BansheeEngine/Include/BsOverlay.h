#pragma once

#include "BsPrerequisites.h"
#include "BsCoreThreadAccessor.h"
#include "BsComponent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Overlay components are a special type of components that can be attached directly
	 * 			to a Camera and used for rendering 2D graphics. Camera will render any of its overlay
	 * 			components after it has rendered the rest of the scene, so these components are usually 
	 * 			used for GUI elements and full screen effects.
	 */
	class BS_EXPORT Overlay : public Component
	{
	public:
		virtual ~Overlay();

		virtual void render(RenderQueue& renderQueue) const = 0;
		virtual void update() {}

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