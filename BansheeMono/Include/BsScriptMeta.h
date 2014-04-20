#pragma once

#include "BsMonoPrerequisites.h"

namespace BansheeEngine
{
	struct BS_MONO_EXPORT ScriptMeta
	{
		ScriptMeta();
		ScriptMeta(const String& assembly, const String& ns, const String& name, std::function<void()> initCallback);

		String ns;
		String name;
		String assembly;
		std::function<void()> initCallback;
		MonoClass* scriptClass;							
		MonoField* thisPtrField;	
	};
}