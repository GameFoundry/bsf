#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "CmD3D9RenderSystem.h"

namespace CamelotFramework
{
	const String SystemName = "D3D9RenderSystem";

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
							newFactory = cm_shared_ptr<D3D9RenderSystemFactory>();
							RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
						}
					}
			};

			static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}