#pragma once

#include "BsForwardRendererPrerequisites.h"
#include "CmRendererFactory.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeForwardRenderer";

	/**
	 * @brief	Renderer factory implementation that creates and initializes the default Banshee renderer.
	 *			Used by the RendererManager.
	 */
	class BS_FWDRND_EXPORT ForwardRendererFactory : public RendererFactory
	{
	public:
		/**
		 * @copydoc	RendererFactory::create
		 */
		virtual RendererPtr create();

		/**
		 * @copydoc	RendererFactory::name
		 */
		virtual const String& name() const;
	};
}