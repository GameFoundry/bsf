#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptTexture.h"
#include "BsScriptObject.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptTextureCube : public ScriptObject <ScriptTextureCube, ScriptTextureBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "TextureCube")

		void* getNativeRaw() const { return mTexture.get(); }

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
			UINT32 height, TextureUsage usage, UINT32 numSamples, bool hasMipmaps, bool gammaCorrection);
		static MonoObject* internal_getPixels(ScriptTextureCube* thisPtr, UINT32 face, UINT32 mipLevel);
		static MonoObject* internal_getGPUPixels(ScriptTextureCube* thisPtr, UINT32 face, UINT32 mipLevel);
		static void internal_setPixels(ScriptTextureCube* thisPtr, MonoObject* data, UINT32 face, UINT32 mipLevel);

		ScriptTextureCube(MonoObject* instance, const HTexture& texture);

		void _onManagedInstanceDeleted();

		HTexture mTexture;
	};
}