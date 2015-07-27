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

		virtual ScriptObjectBackup beginRefresh() override;
		virtual void endRefresh(const ScriptObjectBackup& backupData) override;

	protected:
		bool mRefreshInProgress;
	};

	class BS_SCR_BE_EXPORT ScriptGameObject : public ScriptObject<ScriptGameObject, ScriptGameObjectBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "GameObject")

	private:
		ScriptGameObject(MonoObject* instance);

		static UINT64 internal_getInstanceId(ScriptGameObject* nativeInstance);
	};
}