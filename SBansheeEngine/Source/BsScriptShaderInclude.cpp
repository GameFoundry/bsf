#include "BsScriptShaderInclude.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	ScriptShaderInclude::ScriptShaderInclude(MonoObject* instance, const HShaderInclude& shaderInclude)
		:TScriptResource(instance, shaderInclude)
	{

	}

	void ScriptShaderInclude::initRuntimeData()
	{

	}

	MonoObject* ScriptShaderInclude::createInstance()
	{
		return metaData.scriptClass->createInstance();
	}
}