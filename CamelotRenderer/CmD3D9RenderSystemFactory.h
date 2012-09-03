#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "OgreD3D9RenderSystem.h"

namespace CamelotEngine
{
	const std::string SystemName = "D3D9RenderSystem";

	class D3D9RenderSystemFactory : public RenderSystemFactory
	{
		public:
			virtual RenderSystemPtr create();
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
							newFactory = RenderSystemFactoryPtr(new D3D9RenderSystemFactory());
							RenderSystemManager::registerRenderSystemFactory(newFactory);
						}
					}
			};

			static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}