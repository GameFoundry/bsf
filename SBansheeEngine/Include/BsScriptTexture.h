#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsPixelData.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptTextureBase : public ScriptResourceBase
	{
	protected:
		friend class ScriptResourceManager;

		ScriptTextureBase(MonoObject* instance)
			:ScriptResourceBase(instance)
		{ }

		virtual ~ScriptTextureBase() {}
	};

	class BS_SCR_BE_EXPORT ScriptTexture : public ScriptObject <ScriptTexture, ScriptTextureBase>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "Texture")

	private:
		ScriptTexture(MonoObject* instance);

		static void internal_getPixelFormat(ScriptTexture* thisPtr, PixelFormat* value);
		static void internal_getUsage(ScriptTexture* thisPtr, TextureUsage* value);
		static void internal_getWidth(ScriptTexture* thisPtr, int* value);
		static void internal_getHeight(ScriptTexture* thisPtr, int* value);
		static void internal_getGammaCorrection(ScriptTexture* thisPtr, bool* value);
		static void internal_getSampleCount(ScriptTexture* thisPtr, int* value);
		static void internal_getMipmapCount(ScriptTexture* thisPtr, int* value);
	};
}