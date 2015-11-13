#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptTexture.h"
#include "BsScriptObject.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for a 2D Texture.
	 */
	class BS_SCR_BE_EXPORT ScriptTexture2D : public TScriptResource<ScriptTexture2D, Texture>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Texture2D")

	private:
		friend class ScriptResourceManager;

		ScriptTexture2D(MonoObject* instance, const HTexture& texture);

		/**
		 * @brief	Creates an empty, uninitialized managed instance of the resource interop object.
		 */
		static MonoObject* createInstance();

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
			UINT32 height, TextureUsage usage, UINT32 numSamples, bool hasMipmaps, bool gammaCorrection);
		static MonoObject* internal_getPixels(ScriptTexture2D* thisPtr, UINT32 mipLevel);
		static MonoObject* internal_getGPUPixels(ScriptTexture2D* thisPtr, UINT32 mipLevel);
		static void internal_setPixels(ScriptTexture2D* thisPtr, MonoObject* data, UINT32 mipLevel);
		static void internal_setPixelsArray(ScriptTexture2D* thisPtr, MonoArray* colors, UINT32 mipLevel);
	};
}