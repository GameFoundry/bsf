#pragma once

#include <string>
#include "BsRenderSystemFactory.h"
#include "BsRenderSystemManager.h"
#include "BsD3D9RenderSystem.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeD3D9RenderSystem";

	class D3D9RenderSystemFactory : public RenderSystemFactory
	{
		public:
			virtual void create();
			virtual const String& name() const { return SystemName; }

		private:
			class InitOnStart
			{
				public:
					InitOnStart() 
					{ 
						static RenderSystemFactoryPtr newFactory;
						if(newFactory == nullptr)
						{
							newFactory = bs_shared_ptr<D3D9RenderSystemFactory>();
							RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
						}
					}
			};

			static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}