#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsScriptObject.h"
#include "CmTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptTexture2D : public ScriptResource, public ScriptObject<ScriptTexture2D>
	{
	public:
		static void initMetaData();

		void* getNativeRaw() const { return mTexture.get(); }

		HResource getNativeHandle() const { return mTexture; }
		void setNativeHandle(const HResource& resource);
	private:
		friend class ScriptResourceManager;

		static void internal_createInstance(MonoObject* instance, UINT32 format, UINT32 width, UINT32 height, bool hasMipmaps, bool gammaCorrection);
		static void internal_destroyInstance(ScriptTexture2D* nativeInstance);

		static void initRuntimeData();

		ScriptTexture2D(const HTexture& texture);
		~ScriptTexture2D() {}

		HTexture mTexture;
	};
}