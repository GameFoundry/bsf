#pragma once

#include <string>
#include "BsRenderSystemFactory.h"
#include "BsRenderSystemManager.h"
#include "BsGLRenderSystem.h"

namespace BansheeEngine
{
	const String SystemName = "BansheeGLRenderSystem";

	class GLRenderSystemFactory : public RenderSystemFactory
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
					newFactory = bs_shared_ptr<GLRenderSystemFactory>();
					RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}