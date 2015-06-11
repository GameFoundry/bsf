#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptUnitTests : public ScriptObject <ScriptUnitTests>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "UnitTests")

		static void runTests();

	private:
		static void internal_UT1_GameObjectClone(MonoObject* instance);
		static void internal_UT3_GenerateDiff(MonoObject* oldObj, MonoObject* newObj);
		static void internal_UT3_ApplyDiff(MonoObject* obj);

		static MonoMethod* RunTestsMethod;

		static SPtr<ManagedSerializableDiff> tempDiff;
	};
}