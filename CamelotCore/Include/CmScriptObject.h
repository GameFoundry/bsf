#pragma once

#include "CmPrerequisites.h"
#include <mono/jit/jit.h>

namespace CamelotFramework
{
	/**
	 * @brief	 Base class for objects that can be extended using Mono scripting
	 */
	class CM_EXPORT ScriptObject
	{
	public:
		class CM_EXPORT Meta
		{
			const char* Namespace;								
			const char* Name;									
			MonoClass* Class;									
			MonoDomain* Domain;									
			MonoClassField* ThisPtr;	

			virtual void init() = 0;
		};

		virtual void script_create(MonoObject* scriptObj) = 0;
		virtual void script_destroy() = 0;

		MonoObject* getMonoObject() const { return mScriptObject; }
	private:
		MonoObject* mScriptObject;	
	};
}