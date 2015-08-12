#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptTexture.h"
#include "BsScriptObject.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for a cube Texture.
	 */
	class BS_SCR_BE_EXPORT ScriptTextureCube : public ScriptObject <ScriptTextureCube, ScriptTextureBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "TextureCube")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mTexture; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the native internal cube texture resource.
		 */
		HTexture getTextureHandle() const { return mTexture; }
	private:
		friend class ScriptResourceManager;

		ScriptTextureCube(MonoObject* instance, const HTexture& texture);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HTexture mTexture;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
			UINT32 height, TextureUsage usage, UINT32 numSamples, bool hasMipmaps, bool gammaCorrection);
		static MonoObject* internal_getPixels(ScriptTextureCube* thisPtr, UINT32 face, UINT32 mipLevel);
		static MonoObject* internal_getGPUPixels(ScriptTextureCube* thisPtr, UINT32 face, UINT32 mipLevel);
		static void internal_setPixels(ScriptTextureCube* thisPtr, MonoObject* data, UINT32 face, UINT32 mipLevel);
	};
}