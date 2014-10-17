#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsHandleSlider.h"
#include "BsQuaternion.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleSliderBase : public ScriptObjectBase
	{
	public:
		ScriptHandleSliderBase(MonoObject* managedInstance);
		virtual ~ScriptHandleSliderBase() { }

		virtual HandleSlider* getSlider() const = 0;

	protected:
		friend class ScriptHandleSlider;
		friend class ScriptHandleSliderManager;

		void destroy();
		virtual void destroyInternal() = 0;
	};

	class BS_SCR_BED_EXPORT ScriptHandleSlider : public ScriptObject <ScriptHandleSlider>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "HandleSlider")

	private:
		static void internal_Destroy(ScriptHandleSliderBase* nativeInstance);
		static void internal_GetPosition(ScriptHandleSliderBase* nativeInstance, Vector3* value);
		static void internal_SetPosition(ScriptHandleSliderBase* nativeInstance, Vector3 value);
		static void internal_GetRotation(ScriptHandleSliderBase* nativeInstance, Quaternion* value);
		static void internal_SetRotation(ScriptHandleSliderBase* nativeInstance, Quaternion value);
		static void internal_GetScale(ScriptHandleSliderBase* nativeInstance, Vector3* value);
		static void internal_SetScale(ScriptHandleSliderBase* nativeInstance, Vector3 value);
		static void internal_GetState(ScriptHandleSliderBase* nativeInstance, HandleSlider::State* value);

		ScriptHandleSlider(MonoObject* instance);
	};
}