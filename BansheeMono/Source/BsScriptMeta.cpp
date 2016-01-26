//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
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

	ScriptMeta::ScriptMeta(const String& assembly, const String& ns, const String& name, std::function<void()> initCallback)
		:assembly(assembly), ns(ns), name(name), initCallback(initCallback)
	{

	}
}