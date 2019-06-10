//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsRenderECSFactory.h"
#include "Renderer/BsRenderer.h"
#include "BsRenderECS.h"

namespace bs
{
	constexpr const char* RenderECSFactory::SystemName;

	SPtr<ct::Renderer> RenderECSFactory::create()
	{
		return bs_shared_ptr_new<ct::RenderECS>();
	}

	const String& RenderECSFactory::name() const
	{
		static String StrSystemName = SystemName;
		return StrSystemName;
	}
}
