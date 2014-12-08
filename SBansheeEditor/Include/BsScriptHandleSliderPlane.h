#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptHandleSlider.h"
#include "BsHandleSliderPlane.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleSliderPlane : public ScriptObject <ScriptHandleSliderPlane, ScriptHandleSliderBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "HandleSliderPlane")

	protected:
		virtual HandleSlider* getSlider() const { return mSlider; }
		virtual void destroyInternal();

	private:
		static void internal_CreateInstance(MonoObject* instance, Vector3 dir1, Vector3 dir2, float length, bool fixedScale);
		static void internal_GetDelta(ScriptHandleSliderPlane* nativeInstance, Vector2* value);

		ScriptHandleSliderPlane(MonoObject* instance, const Vector3& dir1, const Vector3& dir2, float length, bool fixedScale);
		~ScriptHandleSliderPlane();

		HandleSliderPlane* mSlider;
	};
}