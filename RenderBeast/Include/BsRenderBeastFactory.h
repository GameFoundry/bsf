#pragma once

#include "BsRenderBeastPrerequisites.h"
#include "BsRendererFactory.h"

namespace BansheeEngine
{
	const String SystemName = "RenderBeast";

	/**
	 * @brief	Renderer factory implementation that creates and initializes the default Banshee renderer.
	 *			Used by the RendererManager.
	 */
	class BS_BSRND_EXPORT RenderBeastFactory : public RendererFactory
	{
	public:
		/**
		 * @copydoc	RendererFactory::create
		 */
		virtual CoreRendererPtr create() override;

		/**
		 * @copydoc	RendererFactory::name
		 */
		virtual const String& name() const override;
	};
}