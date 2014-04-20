#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "CmGLRenderSystem.h"

namespace BansheeEngine
{
	const String SystemName = "GLRenderSystem";

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
					newFactory = cm_shared_ptr<GLRenderSystemFactory>();
					RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}