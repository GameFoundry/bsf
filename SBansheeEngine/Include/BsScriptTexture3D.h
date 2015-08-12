#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptTexture.h"
#include "BsScriptObject.h"
#include "BsTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for 3D Texture.
	 */
	class BS_SCR_BE_EXPORT ScriptTexture3D : public ScriptObject <ScriptTexture3D, ScriptTextureBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Texture3D")

		/**
		 * @copydoc	ScriptResourceBase::getNativeHandle
		 */
		HResource getNativeHandle() const override { return mTexture; }

		/**
		 * @copydoc	ScriptResourceBase::setNativeHandle
		 */
		void setNativeHandle(const HResource& resource) override;

		/**
		 * @brief	Returns the native internal 3D texture resource.
		 */
		HTexture getTextureHandle() const { return mTexture; }
	private:
		friend class ScriptResourceManager;

		ScriptTexture3D(MonoObject* instance, const HTexture& texture);

		/**
		 * @copydoc	ScriptObjectBase::_onManagedInstanceDeleted
		 */
		void _onManagedInstanceDeleted() override;

		HTexture mTexture;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* instance, PixelFormat format, UINT32 width,
			UINT32 height, UINT32 depth, TextureUsage usage, bool hasMipmaps, bool gammaCorrection);
		static MonoObject* internal_getPixels(ScriptTexture3D* thisPtr, UINT32 mipLevel);
		static MonoObject* internal_getGPUPixels(ScriptTexture3D* thisPtr, UINT32 mipLevel);
		static void internal_setPixels(ScriptTexture3D* thisPtr, MonoObject* data, UINT32 mipLevel);
	};
}