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
		virtual void destroy();

	private:
		static void internal_CreateInstance(MonoObject* instance, Vector3 dir1, Vector3 dir2, float length, bool fixedScale, float snapValue);
		static void internal_GetDelta(ScriptHandleSliderPlane* nativeInstance, float* value);
		static void internal_GetDeltaDirection(ScriptHandleSliderPlane* nativeInstance, Vector3* value);
		static void internal_GetNewPosition(ScriptHandleSliderPlane* nativeInstance, Vector3* value);

		ScriptHandleSliderPlane(MonoObject* instance, const Vector3& dir1, const Vector3& dir2, float length, bool fixedScale, float snapValue);
		~ScriptHandleSliderPlane();

		HandleSliderPlane* mSlider;
	};
}