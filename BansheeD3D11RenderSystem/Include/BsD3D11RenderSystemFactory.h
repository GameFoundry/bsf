//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include <string>
#include "BsRenderSystemFactory.h"
#include "BsRenderSystemManager.h"
#include "BsD3D11RenderSystem.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeD3D11RenderSystem";

	/**
	 * @brief	Handles creation of the DX11 render system.
	 */
	class D3D11RenderSystemFactory : public RenderSystemFactory
	{
	public:
		/**
		 * @copydoc	RenderSystemFactory::create
		 */
		virtual void create();

		/**
		 * @copydoc	RenderSystemFactory::name
		 */
		virtual const String& name() const { return SystemName; }

	private:

		/**
		 * @brief	Registers the factory with the render system manager when constructed.
		 */
		class InitOnStart
		{
		public:
			InitOnStart() 
			{ 
				static RenderSystemFactoryPtr newFactory;
				if(newFactory == nullptr)
				{
					newFactory = bs_shared_ptr<D3D11RenderSystemFactory>();
					RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}