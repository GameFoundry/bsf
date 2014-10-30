#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptInputConfiguration : public ScriptObject<ScriptInputConfiguration>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "InputConfiguration")

		InputConfigurationPtr getInternalValue() const { return mInputConfig; }

		static ScriptInputConfiguration* getScriptInputConfig(const InputConfigurationPtr& inputConfig);
		static ScriptInputConfiguration* createScriptInputConfig(const InputConfigurationPtr& inputConfig);

	private:
		static void internal_CreateInstance(MonoObject* object);

		static void internal_RegisterButton(ScriptInputConfiguration* thisPtr, MonoString* name, ButtonCode buttonCode,
			VButtonModifier modifiers, bool repeatable);
		static void internal_UnregisterButton(ScriptInputConfiguration* thisPtr, MonoString* name);

		static void internal_RegisterAxis(ScriptInputConfiguration* thisPtr, MonoString* name, InputAxis type, float deadZone,
			float sensitivity, bool invert);
		static void internal_UnregisterAxis(ScriptInputConfiguration* thisPtr, MonoString* name);

		static void internal_SetRepeatInterval(ScriptInputConfiguration* thisPtr, UINT64 milliseconds);
		static UINT64 internal_GetRepeatInterval(ScriptInputConfiguration* thisPtr);

		ScriptInputConfiguration(MonoObject* instance, const InputConfigurationPtr& inputConfig);

		void _onManagedInstanceDeleted();

		InputConfigurationPtr mInputConfig;

		static Map<UINT64, ScriptInputConfiguration*> ScriptInputConfigurations;
	};

	class BS_SCR_BE_EXPORT ScriptVirtualAxis : public ScriptObject <ScriptVirtualAxis>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "VirtualAxis")

	private:
		static UINT32 internal_InitVirtualAxis(MonoString* name);

		ScriptVirtualAxis(MonoObject* instance);
	};
}