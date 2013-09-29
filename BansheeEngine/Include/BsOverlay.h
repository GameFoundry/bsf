#pragma once

#include "BsPrerequisites.h"
#include "CmCoreThreadAccessor.h"
#include "CmComponent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Overlay components are a special type of components that can be attached directly
	 * 			to a Camera and used for rendering 2D graphics. Camera will render any of its overlay
	 * 			components after it has rendered the rest of the scene, so these components are usually 
	 * 			used for GUI elements and full screen effects.
	 */
	class BS_EXPORT Overlay : public CM::Component
	{
	public:
		virtual ~Overlay();

		virtual void render(CM::RenderQueue& renderQueue) const = 0;
		virtual void update() {}

		CM::Viewport* getTarget() const { return mRenderTarget; }

		/**
		 * @brief	Depth determines in which order the overlays render in case they overlap.
		 * 			Smaller depth means the overlay is in front of anything with larger depth.
		 */
		CM::INT32 getDepth() const { return mDepth; }

		/**
		 * @brief	Depth determines in which order the overlays render in case they overlap.
		 * 			Smaller depth means the overlay is in front of anything with larger depth.
		 */
		void setDepth(CM::INT32 depth) { mDepth = depth; }

	protected:
		friend class CM::SceneObject;

		CM::Viewport* mRenderTarget;
		CM::INT32 mDepth;

		Overlay(const CM::HSceneObject& parent, CM::Viewport* target);
	};
}