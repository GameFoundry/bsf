#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptHandleSlider.h"
#include "BsHandleSliderLine.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT ScriptHandleSliderLine : public ScriptObject <ScriptHandleSliderLine, ScriptHandleSliderBase>
	{
	public:
		SCRIPT_OBJ(BansheeEditorAssemblyName, "BansheeEditor", "HandleSliderLine")

	protected:
		virtual HandleSlider* getSlider() const { return mSlider; }
		virtual void destroy();

	private:
		static void internal_CreateInstance(MonoObject* instance, Vector3 direction, float length, bool fixedScale, float snapValue);
		static void internal_GetDelta(ScriptHandleSliderLine* nativeInstance, float* value);
		static void internal_GetNewPosition(ScriptHandleSliderLine* nativeInstance, Vector3* value);

		ScriptHandleSliderLine(MonoObject* instance, const Vector3& direction, float length, bool fixedScale, float snapValue);
		~ScriptHandleSliderLine();

		HandleSliderLine* mSlider;
	};
}