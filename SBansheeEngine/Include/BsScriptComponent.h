#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"
#include "CmFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptComponent : public ScriptObject<ScriptComponent>
	{
	public:
		static void initMetaData();

	private:
		static void internal_createInstance(MonoObject* instance, MonoString* ns, MonoString* typeName);
		static void internal_destroyInstance(ScriptComponent* nativeInstance);

		static void initRuntimeData();

		ScriptComponent(ManagedComponent* managedComponent);

		ManagedComponent* mManagedComponent;
	};
}