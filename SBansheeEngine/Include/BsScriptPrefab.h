#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptResource.h"
#include "BsPrefab.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptPrefab : public ScriptObject<ScriptPrefab, ScriptResourceBase>
	{
	public:
		SCRIPT_OBJ(ENGINE_ASSEMBLY, "BansheeEngine", "Prefab")

		HResource getNativeHandle() const override { return mPrefab; }
		void setNativeHandle(const HResource& resource);

		HPrefab getPrefabHandle() const { return mPrefab; }
	private:
		friend class ScriptResourceManager;

		ScriptPrefab(MonoObject* instance, const HPrefab& prefab);

		static void internal_CreateInstance(MonoObject* instance, ScriptSceneObject* so);
		static MonoObject* internal_GetRoot(ScriptPrefab* instance);
		static MonoObject* internal_Instantiate(ScriptPrefab* instance);

		void _onManagedInstanceDeleted();

		HPrefab mPrefab;
	};
}