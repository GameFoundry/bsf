//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsBansheeRendererPrerequisites.h"
#include "BsRendererFactory.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeRenderer";

	/**
	 * @brief	Renderer factory implementation that creates and initializes the default Banshee renderer.
	 *			Used by the RendererManager.
	 */
	class BS_BSRND_EXPORT BansheeRendererFactory : public RendererFactory
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