//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsRendererExtension.h"
#include "BsCoreThread.h"
#include "BsRendererManager.h"
#include "BsCoreRenderer.h"

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