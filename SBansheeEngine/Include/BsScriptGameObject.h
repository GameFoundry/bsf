#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObjectBase : public ScriptObjectBase
	{
	public:
		ScriptGameObjectBase(MonoObject* instance)
			:ScriptObjectBase(instance)
		{ }
		virtual ~ScriptGameObjectBase() { }

		virtual HGameObject getNativeHandle() const = 0;
		virtual void setNativeHandle(const HGameObject& gameObject) = 0;
	};

	class BS_SCR_BE_EXPORT ScriptGameObject : public ScriptObject<ScriptGameObject, ScriptGameObjectBase>
	{
	public:
		static String getAssemblyName() { return BansheeEngineAssemblyName; }
		static String getNamespace() { return "BansheeEngine"; }
		static String getTypeName() { return "GameObject"; }
		static void initRuntimeData() { }

	private:
		ScriptGameObject(MonoObject* instance)
			:ScriptObject(instance)
		{ }
	};
}