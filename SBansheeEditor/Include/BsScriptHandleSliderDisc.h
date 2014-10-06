#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptHandleSlider.h"
#include "BsHandleSliderDisc.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleSliderDisc : public ScriptObject <ScriptHandleSliderDisc, ScriptHandleSliderBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "HandleSliderDisc")

	protected:
		virtual HandleSlider* getSlider() const { return mSlider; }
		virtual void destroy();

	private:
		static void internal_CreateInstance(MonoObject* instance, Vector3 normal, float radius, bool fixedScale, float snapValue);
		static void internal_GetDelta(ScriptHandleSliderDisc* nativeInstance, float* value);
		static void internal_GetDeltaDirection(ScriptHandleSliderDisc* nativeInstance, Vector3* value);
		static void internal_GetNewRotation(ScriptHandleSliderDisc* nativeInstance, Quaternion* value);

		ScriptHandleSliderDisc(MonoObject* instance, const Vector3& normal, float radius, bool fixedScale, float snapValue);
		~ScriptHandleSliderDisc();

		HandleSliderDisc* mSlider;
	};
}