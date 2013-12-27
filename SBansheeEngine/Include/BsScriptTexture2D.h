#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmTexture.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptTexture2D : public ScriptObject<ScriptTexture2D>
	{
	public:
		static void initMetaData();

		void* getNativeRaw() const { return mTexture.get(); }

	private:
		static void internal_createInstance(MonoObject* instance, CM::UINT32 format, CM::UINT32 width, CM::UINT32 height, bool hasMipmaps, bool gammaCorrection);
		static void internal_destroyInstance(ScriptTexture2D* nativeInstance);

		static void initRuntimeData();

		ScriptTexture2D(const CM::HTexture& texture);

		CM::HTexture mTexture;
	};
}