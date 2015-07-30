#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptTexture.h"
#include "BsScriptObject.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptTexture3D : public ScriptObject <ScriptTexture3D, ScriptTextureBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Texture3D")

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);

		HTexture getTextureHandle() const { return mTexture; }
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
			UINT32 height, UINT32 depth, TextureUsage usage, bool hasMipmaps, bool gammaCorrection);
		static MonoObject* internal_getPixels(ScriptTexture3D* thisPtr, UINT32 mipLevel);
		static MonoObject* internal_getGPUPixels(ScriptTexture3D* thisPtr, UINT32 mipLevel);
		static void internal_setPixels(ScriptTexture3D* thisPtr, MonoObject* data, UINT32 mipLevel);

		ScriptTexture3D(MonoObject* instance, const HTexture& texture);

		void _onManagedInstanceDeleted();

		HTexture mTexture;
	};
}