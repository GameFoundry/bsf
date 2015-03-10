#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "BsInputConfiguration.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptVirtualButton : public ScriptObject <ScriptVirtualButton>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "VirtualButton")

		static VirtualButton unbox(MonoObject* obj);
		static MonoObject* box(const VirtualButton& value);

	private:
		static UINT32 internal_InitVirtualButton(MonoString* name);

		ScriptVirtualButton(MonoObject* instance);
	};
}