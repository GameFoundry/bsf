#pragma once

#include <string>
#include "BsRenderAPIFactory.h"
#include "BsRenderAPIManager.h"
#include "BsD3D9RenderAPI.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeD3D9RenderSystem";

	/**
	 * @brief	Handles creation of the DX9 render system.
	 */
	class D3D9RenderAPIFactory : public RenderAPIFactory
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
					static RenderAPIFactoryPtr newFactory;
					if(newFactory == nullptr)
					{
						newFactory = bs_shared_ptr_new<D3D9RenderAPIFactory>();
						RenderAPIManager::instance().registerFactory(newFactory);
					}
				}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}