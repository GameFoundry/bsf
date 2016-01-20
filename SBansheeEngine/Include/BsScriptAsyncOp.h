//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for AsyncOp.
	 */
	class BS_SCR_BE_EXPORT ScriptAsyncOp : public ScriptObject<ScriptAsyncOp>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "AsyncOp")

		/**
		 * @brief	Creates a new managed AsyncOp instance.
		 *
		 * @param	op						Native AsyncOp to wrap.
		 * @param	asyncOpToReturnValue	Callback that converts the returned value from native async op
		 *									to a managed object.
		 */
		static MonoObject* create(const AsyncOp& op, std::function<MonoObject*(const AsyncOp&)> asyncOpToReturnValue);

	private:
		ScriptAsyncOp(MonoObject* instance);

		/**
		 * @brief	Finishes construction of the AsyncOp wrapper. Must be called before using the object.
		 */
		void initialize(const AsyncOp& op, std::function<MonoObject*(const AsyncOp&)> asyncOpToReturnValue);

		AsyncOp mAsyncOp;
		std::function<MonoObject*(const AsyncOp&)> mConvertCallback;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_createInstance(MonoObject* managedInstance);
		static void internal_isComplete(ScriptAsyncOp* thisPtr, bool* value);
		static MonoObject* internal_getReturnValue(ScriptAsyncOp* thisPtr);
		static void internal_blockUntilComplete(ScriptAsyncOp* thisPtr);
	};
}