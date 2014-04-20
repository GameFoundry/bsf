#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{
	class BS_MONO_EXPORT MonoMethod
	{
	public:
		MonoObject* invoke(MonoObject* instance, void** params);
		MonoObject* invokeVirtual(MonoObject* instance, void** params);

		/**
		 * @brief	Gets a thunk for this method. A thunk is a C++ like function
		 * 			pointer that you can use for calling the method.
		 *
		 * @note	This is the fastest way of calling managed code.
		 */
		void* getThunk() const;

		CM::String getName() const;

		MonoClass* getReturnType();

	private:
		friend class MonoClass;
		friend class MonoProperty;

		MonoMethod(::MonoMethod* method);

		::MonoMethod* mMethod;
		MonoClass* mReturnType;
		void* mThunk;
	};
}