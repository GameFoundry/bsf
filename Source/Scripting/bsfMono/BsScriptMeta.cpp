//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsScriptMeta.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoField.h"

namespace bs
{
	ScriptMeta::ScriptMeta()
		:scriptClass(nullptr), thisPtrField(nullptr)
	{

	}

	ScriptMeta::ScriptMeta(const String& assembly, const String& ns, const String& name, std::function<void()> initCallback)
		:ns(ns), name(name), assembly(assembly), initCallback(initCallback), scriptClass(nullptr), thisPtrField(nullptr)
	{

	}
}
