#pragma once

#include "BsPrerequisites.h"
#include "CmDeferredRenderContext.h"
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

		virtual void render(const Camera* camera, CM::RenderContext& renderContext) const = 0;
		virtual void update() {}

	protected:
		friend class CM::SceneObject;

		Overlay(const CM::HSceneObject& parent);
	};
}