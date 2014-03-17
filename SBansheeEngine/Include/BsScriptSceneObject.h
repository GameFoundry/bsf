#pragma once

#include "BsScriptEnginePrerequisites.h"
#include "BsScriptGameObject.h"
#include "BsScriptObject.h"
#include "CmFont.h"

namespace BansheeEngine
{
	class BS_SCR_BE_EXPORT ScriptSceneObject : public ScriptGameObject, public ScriptObject<ScriptSceneObject>
	{
	public:
		static void initMetaData();

		virtual CM::HGameObject getNativeHandle() const { return mSceneObject; }
		virtual void setNativeHandle(const CM::HGameObject& gameObject);

	private:
		friend class ScriptGameObjectManager;

		static void internal_createInstance(MonoObject* instance, MonoString* name);
		static void internal_destroyInstance(ScriptSceneObject* nativeInstance);

		static void internal_setParent(ScriptSceneObject* nativeInstance, MonoObject* parent);
		static MonoObject* internal_getParent(ScriptSceneObject* nativeInstance);

		static CM::UINT32 internal_getNumChildren(ScriptSceneObject* nativeInstance);
		static MonoObject* internal_getChild(ScriptSceneObject* nativeInstance, CM::UINT32 idx);

		static void initRuntimeData();

		ScriptSceneObject(const CM::HSceneObject& sceneObject);
		~ScriptSceneObject() {}

		CM::HSceneObject mSceneObject;
	};
}