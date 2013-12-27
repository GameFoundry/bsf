#pragma once

#include "BsMonoPrerequisites.h"

namespace BansheeEngine
{
	struct BS_MONO_EXPORT ScriptMeta
	{
		ScriptMeta();
		ScriptMeta(const CM::String& assembly, const CM::String& ns, const CM::String& name, std::function<void()> initCallback);

		CM::String ns;
		CM::String name;
		CM::String assembly;
		std::function<void()> initCallback;
		MonoClass* scriptClass;							
		MonoField* thisPtrField;	
	};
}