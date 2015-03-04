#include "BsRenderer.h"
#include "BsRendererManager.h"

namespace BansheeEngine
{
	GpuLanguage Renderer::getGpuLanguageType(const String& name)
	{
		if (name == "hlsl")
			return GpuLanguage::HLSL;
		else if (name == "glsl")
			return GpuLanguage::GLSL;

		return GpuLanguage::Undefined;
	}

	String Renderer::getGpuLanguageName(GpuLanguage language)
	{
		switch (language)
		{
		case GpuLanguage::HLSL:
			return "hlsl";
		case GpuLanguage::GLSL:
			return "glsl";
		}

		return "";
	}

	SPtr<Renderer> gRenderer()
	{
		return std::static_pointer_cast<Renderer>(RendererManager::instance().getActive());
	}
}