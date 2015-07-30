#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptTexture.h"
#include "BsScriptObject.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptTexture2D : public ScriptObject<ScriptTexture2D, ScriptTextureBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Texture2D")

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);

		HTexture getTextureHandle() const { return mTexture; }
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width, 
			UINT32 height, TextureUsage usage, UINT32 numSamples, bool hasMipmaps, bool gammaCorrection);
		static MonoObject* internal_getPixels(ScriptTexture2D* thisPtr, UINT32 mipLevel);
		static MonoObject* internal_getGPUPixels(ScriptTexture2D* thisPtr, UINT32 mipLevel);
		static void internal_setPixels(ScriptTexture2D* thisPtr, MonoObject* data, UINT32 mipLevel);
		static void internal_setPixelsArray(ScriptTexture2D* thisPtr, MonoArray* colors, UINT32 mipLevel);

		ScriptTexture2D(MonoObject* instance, const HTexture& texture);

		void _onManagedInstanceDeleted();

		HTexture mTexture;
	};
}