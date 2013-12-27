#include "BsScriptMeta.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"

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