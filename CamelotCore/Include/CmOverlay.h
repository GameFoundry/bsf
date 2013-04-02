#pragma once

#include "CmPrerequisites.h"
#include "CmComponent.h"

namespace CamelotEngine
{
	/**
	 * @brief	Overlay components are a special type of components that can be attached directly
	 * 			to a Camera and used for rendering 2D graphics. Camera will render any of its overlay
	 * 			components after it has rendered the rest of the scene, so these components are usually 
	 * 			used for GUI elements and full screen effects.
	 */
	class CM_EXPORT Overlay : public Component
	{
	public:
		virtual ~Overlay();

		virtual void render(const CameraPtr& camera, DeferredRenderContextPtr& renderContext) const = 0;

	protected:
		friend class GameObject;

		Overlay(GameObjectPtr parent);
	};
}