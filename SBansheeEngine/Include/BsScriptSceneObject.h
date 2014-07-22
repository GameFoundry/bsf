#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "BsFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSceneObject : public ScriptObject<ScriptSceneObject, ScriptGameObjectBase>
	{
	public:
		SCRIPT_OBJ(BansheeEngineAssemblyName, "BansheeEngine", "SceneObject")

		virtual HGameObject getNativeHandle() const { return mSceneObject; }
		virtual void setNativeHandle(const HGameObject& gameObject);

	private:
		friend class ScriptGameObjectManager;

		static void internal_createInstance(MonoObject* instance, MonoString* name);

		static void internal_setParent(ScriptSceneObject* nativeInstance, MonoObject* parent);
		static MonoObject* internal_getParent(ScriptSceneObject* nativeInstance);

		static UINT32 internal_getNumChildren(ScriptSceneObject* nativeInstance);
		static MonoObject* internal_getChild(ScriptSceneObject* nativeInstance, UINT32 idx);

		ScriptSceneObject(MonoObject* instance, const HSceneObject& sceneObject);

		void _onManagedInstanceDeleted();

		HSceneObject mSceneObject;
	};
}