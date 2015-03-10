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
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "HandleSliderDisc")

	protected:
		virtual HandleSlider* getSlider() const { return mSlider; }
		virtual void destroyInternal();

	private:
		static void internal_CreateInstance(MonoObject* instance, Vector3 normal, float radius, bool fixedScale);
		static void internal_GetDelta(ScriptHandleSliderDisc* nativeInstance, float* value);
		static void internal_GetStartAngle(ScriptHandleSliderDisc* nativeInstance, float* value);
		static void internal_SetCutoffPlane(ScriptHandleSliderDisc* nativeInstance, float value, bool enabled);

		ScriptHandleSliderDisc(MonoObject* instance, const Vector3& normal, float radius, bool fixedScale);
		~ScriptHandleSliderDisc();

		HandleSliderDisc* mSlider;
	};
}