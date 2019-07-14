//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsNullRenderer.h"
#include "BsCoreApplication.h"
#include "CoreThread/BsCoreThread.h"
#include "Renderer/BsRendererManager.h"
#include "CoreThread/BsCoreObjectManager.h"

using namespace std::placeholders;

namespace bs
{
	constexpr const char* NullRendererFactory::SystemName;

	SPtr<ct::Renderer> NullRendererFactory::create()
	{
		return bs_shared_ptr_new<ct::NullRenderer>();
	}

	const String& NullRendererFactory::name() const
	{
		static String StrSystemName = SystemName;
		return StrSystemName;
	}

	namespace ct
	{
		const StringID& NullRenderer::getName() const
		{
			static StringID name = "NullRenderer";
			return name;
		}

		void NullRenderer::renderAll(PerFrameData perFrameData)
		{
			CoreObjectManager::instance().syncToCore();
		}

		SPtr<NullRenderer> gNullRenderer()
		{
			return std::static_pointer_cast<NullRenderer>(RendererManager::instance().getActive());
		}
	}
}
