#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "OgreGLRenderSystem.h"

namespace CamelotEngine
{
	const std::string SystemName = "GLRenderSystem";

	class GLRenderSystemFactory : public RenderSystemFactory
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
					newFactory = RenderSystemFactoryPtr(new GLRenderSystemFactory());
					RenderSystemManager::registerRenderSystemFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}