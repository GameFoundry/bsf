#pragma once

#include "BsPrerequisites.h"
#include "BsScriptObject.h"
#include "CmTexture.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptTexture : public ScriptObject<ScriptTexture>
	{
	public:
		static void initMetaData();

	private:
		static void internal_createInstance(MonoObject* instance, CM::UINT32 format, CM::UINT32 width, CM::UINT32 height, bool hasMipmaps, bool gammaCorrection);
		static void internal_destroyInstance(ScriptTexture* nativeInstance);

		static void initRuntimeData();

		ScriptTexture(const CM::HTexture& texture);

		CM::HTexture mTexture;
	};
}