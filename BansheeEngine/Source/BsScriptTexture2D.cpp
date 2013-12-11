#include "BsScriptTexture2D.h"
#include "BsScriptMeta.h"
#include "BsScriptField.h"
#include "BsScriptClass.h"
#include "BsScriptManager.h"
#include "CmTexture.h"
#include "CmPixelUtil.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	ScriptTexture::ScriptTexture(const CM::HTexture& texture)
		:mTexture(texture)
	{

	}

	void ScriptTexture::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "Texture2D", &ScriptTexture::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptTexture::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptTexture::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptTexture::internal_destroyInstance);
	}

	void ScriptTexture::internal_createInstance(MonoObject* instance, CM::UINT32 format, CM::UINT32 width, CM::UINT32 height, bool hasMipmaps, bool gammaCorrection)
	{
		PixelFormat texFormat = PF_R8G8B8;
		switch(format)
		{
		case 0: // RGB
			texFormat = PF_R8G8B8;
			break;
		case 1: // RGBA
			texFormat = PF_R8G8B8A8;
			break;
		default:
			CM_EXCEPT(InvalidParametersException, "Unsupported texture format");
		}

		int numMips = 0;
		if(hasMipmaps)
			numMips = PixelUtil::getMaxMipmaps(width, height, 1, texFormat);

		HTexture texture = Texture::create(TEX_TYPE_2D, width, height, numMips, texFormat, TU_STATIC, gammaCorrection);

		ScriptTexture* nativeInstance = new (cm_alloc<ScriptTexture>()) ScriptTexture(texture);
		nativeInstance->createInstance();

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptTexture::internal_destroyInstance(ScriptTexture* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}