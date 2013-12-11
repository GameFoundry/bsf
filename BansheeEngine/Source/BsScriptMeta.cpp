#include "BsScriptMeta.h"
#include "BsScriptManager.h"
#include "BsScriptClass.h"
#include "BsScriptField.h"
#include "BsEngineAssembly.h"

namespace BansheeEngine
{
	ScriptMeta::ScriptMeta()
		:scriptClass(nullptr), thisPtrField(nullptr)
	{

	}

	ScriptMeta::ScriptMeta(const CM::String& assembly, const CM::String& ns, const CM::String& name, std::function<void()> initCallback)
		:assembly(assembly), ns(ns), name(name), initCallback(initCallback)
	{

	}
}