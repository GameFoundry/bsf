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
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "HandleSliderLine")

	protected:
		virtual HandleSlider* getSlider() const { return mSlider; }
		virtual void destroyInternal();

	private:
		static void internal_CreateInstance(MonoObject* instance, Vector3 direction, float length, bool fixedScale);
		static void internal_GetDelta(ScriptHandleSliderLine* nativeInstance, float* value);

		ScriptHandleSliderLine(MonoObject* instance, const Vector3& direction, float length, bool fixedScale);
		~ScriptHandleSliderLine();

		HandleSliderLine* mSlider;
	};
}