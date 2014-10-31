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

		static void internal_getPosition(ScriptSceneObject* nativeInstance, Vector3* value);
		static void internal_getLocalPosition(ScriptSceneObject* nativeInstance, Vector3* value);
		static void internal_getRotation(ScriptSceneObject* nativeInstance, Quaternion* value);
		static void internal_getLocalRotation(ScriptSceneObject* nativeInstance, Quaternion* value);
		static void internal_getScale(ScriptSceneObject* nativeInstance, Vector3* value);
		static void internal_getLocalScale(ScriptSceneObject* nativeInstance, Vector3* value);

		static void internal_setPosition(ScriptSceneObject* nativeInstance, Vector3 value);
		static void internal_setLocalPosition(ScriptSceneObject* nativeInstance, Vector3 value);
		static void internal_setRotation(ScriptSceneObject* nativeInstance, Quaternion value);
		static void internal_setLocalRotation(ScriptSceneObject* nativeInstance, Quaternion value);
		static void internal_setLocalScale(ScriptSceneObject* nativeInstance, Vector3 value);

		static void internal_getLocalTransform(ScriptSceneObject* nativeInstance, Matrix4* value);
		static void internal_getWorldTransform(ScriptSceneObject* nativeInstance, Matrix4* value);
		static void internal_lookAt(ScriptSceneObject* nativeInstance, Vector3 direction, Vector3 up);
		static void internal_move(ScriptSceneObject* nativeInstance, Vector3 value);
		static void internal_moveLocal(ScriptSceneObject* nativeInstance, Vector3 value);
		static void internal_rotate(ScriptSceneObject* nativeInstance, Quaternion value);
		static void internal_roll(ScriptSceneObject* nativeInstance, Radian value);
		static void internal_yaw(ScriptSceneObject* nativeInstance, Radian value);
		static void internal_pitch(ScriptSceneObject* nativeInstance, Radian value);
		static void internal_setForward(ScriptSceneObject* nativeInstance, Vector3 value);
		static void internal_getForward(ScriptSceneObject* nativeInstance, Vector3* value);
		static void internal_getUp(ScriptSceneObject* nativeInstance, Vector3* value);
		static void internal_getRight(ScriptSceneObject* nativeInstance, Vector3* value);

		static void internal_destroy(ScriptSceneObject* nativeInstance, bool immediate);
		static bool checkIfDestroyed(ScriptSceneObject* nativeInstance);

		ScriptSceneObject(MonoObject* instance, const HSceneObject& sceneObject);

		void _onManagedInstanceDeleted();

		HSceneObject mSceneObject;
	};
}