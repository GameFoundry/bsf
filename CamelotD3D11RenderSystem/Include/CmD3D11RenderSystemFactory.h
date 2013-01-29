#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "CmD3D11RenderSystem.h"

namespace CamelotEngine
{
	const std::string SystemName = "D3D11RenderSystem";

	class D3D11RenderSystemFactory : public RenderSystemFactory
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
					newFactory = RenderSystemFactoryPtr(new D3D11RenderSystemFactory());
					RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}