#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptObject.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptGameObjectBase : public PersistentScriptObjectBase
	{
	public:
		ScriptGameObjectBase(MonoObject* instance);
		virtual ~ScriptGameObjectBase() { }

		virtual HGameObject getNativeHandle() const = 0;
		virtual void setNativeHandle(const HGameObject& gameObject) = 0;

		virtual ScriptObjectBackup beginRefresh();
		virtual void endRefresh(const ScriptObjectBackup& backupData);

	protected:
		bool mRefreshInProgress;
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