//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsRendererManager.h"
#include "BsRenderer.h"
#include "BsException.h"

namespace BansheeEngine
{
	RendererManager::~RendererManager()
	{
		if (mActiveRenderer != nullptr)
			mActiveRenderer->_onDeactivated();
	}

	void RendererManager::setActive(const String& name)
	{
		for(auto iter = mAvailableFactories.begin(); iter != mAvailableFactories.end(); ++iter)
		{
			if((*iter)->name() == name)
			{
				RendererPtr newRenderer = (*iter)->create();
				if(newRenderer != nullptr)
				{
					if (mActiveRenderer != nullptr)
						mActiveRenderer->_onDeactivated();

					mActiveRenderer = newRenderer;
					mActiveRenderer->_onActivated();
				}				
			}
		}

		if(mActiveRenderer == nullptr)
		{
			BS_EXCEPT(InternalErrorException, 
				"Cannot initialize renderer. Renderer with the name '" + name + "' cannot be found.")
		}
	}

	const String& RendererManager::getCoreRendererName()
	{
		static String name = "CoreRenderer";
		return name;
	}

	void RendererManager::_registerFactory(RendererFactoryPtr factory)
	{
		assert(factory != nullptr);

		mAvailableFactories.push_back(factory);
	}
}