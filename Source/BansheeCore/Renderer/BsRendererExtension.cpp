//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Renderer/BsRendererExtension.h"
#include "CoreThread/BsCoreThread.h"
#include "Renderer/BsRendererManager.h"
#include "Renderer/BsRenderer.h"

namespace bs
{
	void RendererExtension::_initializer(RendererExtension* obj, const Any& data)
	{
		auto coreInitializer = [=]()
		{
			RendererManager::instance().getActive()->addPlugin(obj);
			obj->initialize(data);
		};

		gCoreThread().queueCommand(coreInitializer);
	}

	void RendererExtension::_deleter(RendererExtension* obj)
	{
		auto deleteObj = [=]()
		{
			RendererManager::instance().getActive()->removePlugin(obj);

			obj->destroy();
			obj->~RendererExtension();

			bs_free(obj);
		};

		// Queue deletion on the core thread
		gCoreThread().queueCommand(deleteObj);
	}
}