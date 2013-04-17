#pragma once

#include <string>
#include "CmRenderSystemFactory.h"
#include "CmRenderSystemManager.h"
#include "CmGLRenderSystem.h"

namespace CamelotFramework
{
	const std::string SystemName = "GLRenderSystem";

	class GLRenderSystemFactory : public RenderSystemFactory
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
					newFactory = RenderSystemFactoryPtr(CM_NEW(GLRenderSystemFactory, GenAlloc) GLRenderSystemFactory(),
						&MemAllocDeleter<GLRenderSystemFactory, GenAlloc>::deleter);
					RenderSystemManager::instance().registerRenderSystemFactory(newFactory);
				}
			}
		};

		static InitOnStart initOnStart; // Makes sure factory is registered on program start
	};
}