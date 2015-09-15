#include "BsScriptBuiltin.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsBuiltinResources.h"
#include "BsScriptSpriteTexture.h"
#include "BsScriptShader.h"
#include "BsScriptResourceManager.h"

namespace BansheeEngine
{
	ScriptBuiltin::ScriptBuiltin(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptBuiltin::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetWhiteTexture", &ScriptBuiltin::internal_GetWhiteTexture);
		metaData.scriptClass->addInternalCall("Internal_GetDiffuseShader", &ScriptBuiltin::internal_GetDiffuseShader);
	}

	MonoObject* ScriptBuiltin::internal_GetWhiteTexture()
	{
		HSpriteTexture whiteTexture = BuiltinResources::instance().getWhiteSpriteTexture();

		ScriptSpriteTexture* scriptSpriteTex;
		ScriptResourceManager::instance().getScriptResource(whiteTexture, &scriptSpriteTex, true);

		return scriptSpriteTex->getManagedInstance();
	}

	MonoObject* ScriptBuiltin::internal_GetDiffuseShader()
	{
		HShader diffuseShader = BuiltinResources::instance().getDiffuseShader();

		ScriptShader* scriptShader;
		ScriptResourceManager::instance().getScriptResource(diffuseShader, &scriptShader, true);

		return scriptShader->getManagedInstance();
	}
}