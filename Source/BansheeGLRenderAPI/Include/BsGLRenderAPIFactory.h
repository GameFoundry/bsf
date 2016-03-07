//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include <string>
#include "BsRenderAPIFactory.h"
#include "BsRenderAPIManager.h"
#include "BsGLRenderAPI.h"

namespace BansheeEngine
{
	/** @addtogroup GL
	 *  @{
	 */

	const String SystemName = "BansheeGLRenderSystem";

	/** Handles creation of the OpenGL render system. */
	class GLRenderAPIFactory : public RenderAPIFactory
	{
	public:
		/** @copydoc RenderSystemFactory::create */
		void create() override;

		/** @copydoc RenderSystemFactory::name */
		const String& name() const override { return SystemName; }

	private:
		/**	Registers the factory with the render system manager when constructed. */
		class InitOnStart
		{
		public:
			InitOnStart() 
			{ 
				static RenderAPIFactoryPtr newFactory;
				if(newFactory == nullptr)
				{
					newFactory = bs_shared_ptr_new<GLRenderAPIFactory>();
					RenderAPIManager::instance().registerFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on library load
	};

	/** @} */
}