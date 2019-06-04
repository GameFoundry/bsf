//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsECSRenderer.h"
#include "BsCoreApplication.h"
#include "CoreThread/BsCoreThread.h"
#include "Renderer/BsRendererManager.h"
#include "CoreThread/BsCoreObjectManager.h"

using namespace std::placeholders;

namespace bs
{
	constexpr const char* ECSRendererFactory::SystemName;

	SPtr<ct::Renderer> ECSRendererFactory::create()
	{
		return bs_shared_ptr_new<ct::ECSRenderer>();
	}

	const String& ECSRendererFactory::name() const
	{
		static String StrSystemName = SystemName;
		return StrSystemName;
	}

	namespace ct
	{
		const StringID& ECSRenderer::getName() const
		{
			static StringID name = "ECSRenderer";
			return name;
		}

		void ECSRenderer::renderAll(PerFrameData perFrameData)
		{
			CoreObjectManager::instance().syncToCore();
		}

		SPtr<ECSRenderer> gECSRenderer()
		{
			return std::static_pointer_cast<ECSRenderer>(RendererManager::instance().getActive());
		}
	}
}