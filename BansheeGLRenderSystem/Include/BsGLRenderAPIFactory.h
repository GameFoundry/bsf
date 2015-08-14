#pragma once

#include <string>
#include "BsRenderAPIFactory.h"
#include "BsRenderAPIManager.h"
#include "BsGLRenderAPI.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeGLRenderSystem";

	/**
	 * @brief Handles creation of the OpenGL render system.
	 */
	class GLRenderAPIFactory : public RenderAPIFactory
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
					newFactory = bs_shared_ptr_new<GLRenderAPIFactory>();
					RenderAPIManager::instance().registerFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on library load
	};
}