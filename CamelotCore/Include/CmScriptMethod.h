#pragma once

#include "CmPrerequisites.h"
#include <mono/jit/jit.h>

namespace CamelotFramework
{
	class CM_EXPORT ScriptMethod
	{
	public:
		MonoObject* invoke(ScriptObject* instance, void** params);

		/**
		 * @brief	Gets a thunk for this method. A thunk is a C++ like function
		 * 			pointer that you can use for calling the method.
		 *
		 * @note	This is the fastest way of calling managed code.
		 */
		void* getThunk();

	private:
		friend class ScriptClass;

		ScriptMethod(MonoMethod* method);

		MonoMethod* mMethod;
		void* mThunk;
	};
}