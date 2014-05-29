#pragma once

#include "BsMonoPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains information required for initializing and handling a single script class.
	 */
	struct BS_MONO_EXPORT ScriptMeta
	{
		ScriptMeta();
		ScriptMeta(const String& assembly, const String& ns, const String& name, std::function<void()> initCallback);

		String ns; /**< Namespace the script class is located in. */
		String name; /**< Type name of the script class. */
		String assembly; /**< Name of the assembly the script class is located in. */
		std::function<void()> initCallback; /**< Callback that will be triggered when assembly containing the class is loaded. Used for one time initialization. */
		MonoClass* scriptClass; /**< Class object describing the script class. Only valid after assembly containing this type was loaded. */			
		MonoField* thisPtrField; /**< Field object that contains a pointer to the native instance of the script object. Only valid after assembly containing this type was loaded. */
	};
}