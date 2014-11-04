#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsAsyncOp.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptAsyncOp : public ScriptObject<ScriptAsyncOp>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "AsyncOp")

		static MonoObject* create(const AsyncOp& op, std::function<MonoObject*(const AsyncOp&)> asyncOpToReturnValue);

	private:
		ScriptAsyncOp(MonoObject* instance);
		void initialize(const AsyncOp& op, std::function<MonoObject*(const AsyncOp&)> asyncOpToReturnValue);

		static void internal_createInstance(MonoObject* managedInstance);
		static void internal_isComplete(ScriptAsyncOp* thisPtr, bool* value);
		static MonoObject* internal_getReturnValue(ScriptAsyncOp* thisPtr);
		static void internal_blockUntilComplete(ScriptAsyncOp* thisPtr);

		AsyncOp mAsyncOp;
		std::function<MonoObject*(const AsyncOp&)> mConvertCallback;
	};
}