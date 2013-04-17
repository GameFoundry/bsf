#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "CmD3D9RenderSystem.h"

namespace CamelotFramework
{
	const std::string SystemName = "D3D9RenderSystem";

	class D3D9RenderSystemFactory : public RenderSystemFactory
	{
		public:
			virtual void create();
			virtual const std::string& name() const { return SystemName; }

		private:
			class InitOnStart
			{
				public:
					InitOnStart() 
					{ 
						static RenderSystemFactoryPtr newFactory;
						if(newFactory == nullptr)
						{
							newFactory = RenderSystemFactoryPtr(CM_NEW(D3D9RenderSystemFactory, GenAlloc) D3D9RenderSystemFactory(),
								&MemAllocDeleter<D3D9RenderSystemFactory, GenAlloc>::deleter);
							RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
						}
					}
			};

			static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}