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
	ScriptTexture2D::ScriptTexture2D(const CM::HTexture& texture)
		:mTexture(texture)
	{

	}

	void ScriptTexture2D::initMetaData()
	{
		metaData = ScriptMeta("MBansheeEngine", "BansheeEngine", "Texture2D", &ScriptTexture2D::initRuntimeData);

		ScriptManager::registerScriptType(&metaData);
	}

	void ScriptTexture2D::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptTexture2D::internal_createInstance);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptTexture2D::internal_destroyInstance);
	}

	void ScriptTexture2D::internal_createInstance(MonoObject* instance, CM::UINT32 format, CM::UINT32 width, CM::UINT32 height, bool hasMipmaps, bool gammaCorrection)
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

		ScriptTexture2D* nativeInstance = new (cm_alloc<ScriptTexture2D>()) ScriptTexture2D(texture);
		nativeInstance->createInstance(instance);

		metaData.thisPtrField->setValue(instance, nativeInstance);
	}

	void ScriptTexture2D::internal_destroyInstance(ScriptTexture2D* nativeInstance)
	{
		nativeInstance->destroyInstance();
		cm_delete(nativeInstance);
	}
}