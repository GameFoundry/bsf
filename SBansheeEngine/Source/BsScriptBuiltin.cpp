#include "BsScriptBuiltin.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsBuiltinResources.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptResourceManager.h"

namespace BansheeEngine
{
	ScriptBuiltin::ScriptBuiltin(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptBuiltin::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetWhiteTexture", &ScriptBuiltin::internal_getWhiteTexture);
	}

	MonoObject* ScriptBuiltin::internal_getWhiteTexture()
	{
		HSpriteTexture whiteTexture = BuiltinResources::instance().getWhiteSpriteTexture();

		ScriptSpriteTexture* scriptSpriteTex = ScriptResourceManager::instance().getScriptSpriteTexture(whiteTexture);
		if (scriptSpriteTex == nullptr)
			scriptSpriteTex = ScriptResourceManager::instance().createScriptSpriteTexture(whiteTexture);

		return scriptSpriteTex->getManagedInstance();
	}
}