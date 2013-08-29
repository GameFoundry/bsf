#pragma once

#include "CmPrerequisites.h"
#include "CmGameObject.h"
#include <boost/signals.hpp>

namespace CamelotFramework
{
	class CM_EXPORT Renderer
	{
	public:
		virtual const String& getName() const = 0;

		/**
		 * @brief	Renders all cameras.
		 */
		virtual void renderAll() = 0;

		void addRenderCallback(const Viewport* viewport, std::function<void(const Viewport*, RenderQueue&)> callback);

	protected:
		UnorderedMap<const Viewport*, Vector<std::function<void(const Viewport*, RenderQueue&)>>::type>::type mRenderCallbacks;
	};
}