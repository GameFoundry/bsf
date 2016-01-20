//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsMonoPrerequisites.h"
#include <mono/jit/jit.h>

namespace BansheeEngine
{

	/**
	 * @brief	Encapsulates information about a single Mono (i.e. managed) method
	 *			belonging to some managed class. This object also allows you to invoke
	 *			the method.
	 */
	class BS_MONO_EXPORT MonoMethod
	{
	public:
		~MonoMethod();

		/**
		 * @brief	Invokes the method on the provided object instance. 
		 *			This does not respect polymorphism and will invoke the exact method
		 *			of the class this object was retrieved from. Use "invokeVirtual" if you
		 *			need polymorphism.
		 *
		 * @param	instance	Instance of the object to invoke the method on. Can be null for static methods.
		 * @param	param		Array of parameters to pass to the method. Caller must ensure they match method 
		 *						parameter count and type. For value types parameters should be pointers to the 
		 *						values and for reference types they should be pointers to MonoObject.
		 *
		 @returns	A boxed return value, or null if method has no return value.
		 */
		MonoObject* invoke(MonoObject* instance, void** params);

		/**
		 * @brief	Invokes the method on the provided object instance. If the instance has an override of this
		 *			method it will be called.
		 *
		 * @param	instance	Instance of the object to invoke the method on.
		 * @param	param		Array of parameters to pass to the method. Caller must ensure they match method 
		 *						parameter count and type. For value types parameters should be pointers to the 
		 *						values and for reference types they should be pointers to MonoObject.
		 *
		 * @returns	A boxed return value, or null if method has no return value.
		 */
		MonoObject* invokeVirtual(MonoObject* instance, void** params);

		/**
		 * @brief	Gets a thunk for this method. A thunk is a C++ like function
		 * 			pointer that you can use for calling the method.
		 *
		 * @note	This is the fastest way of calling managed code.
		 */
		void* getThunk() const;

		/**
		 * @brief	Returns the name of the method.
		 */
		String getName() const;

		/**
		 * @brief	Returns the type of the return value. Returns null if method
		 *			has no return value.
		 */
		MonoClass* getReturnType() const;

		/**
		 * @brief	Returns the number of parameters the method expects.
		 */
		UINT32 getNumParameters() const;

		/**
		 * @brief	Returns the type of the method parameter at the specified index.
		 */
		MonoClass* getParameterType(UINT32 paramIdx) const;

		/**
		 * @brief	Returns true if the method doesn't require a class instance.
		 */
		bool isStatic() const;

		/**
		 * @brief	Checks if method has an attribute of the specified type.
		 */
		 bool hasAttribute(MonoClass* monoClass) const;

		/**
		 * @brief	Returns an instance of an attribute of the specified type. Returns null
		 *			if the method doesn't have such an attribute.
		 */
		MonoObject* getAttribute(MonoClass* monoClass) const;

	private:
		friend class MonoClass;
		friend class MonoProperty;

		MonoMethod(::MonoMethod* method);

		void cacheSignature() const;

		::MonoMethod* mMethod;

		mutable MonoClass* mCachedReturnType;
		mutable MonoClass** mCachedParameters;
		mutable UINT32 mCachedNumParameters;
		mutable bool mIsStatic;
		mutable bool mHasCachedSignature;
	};
}