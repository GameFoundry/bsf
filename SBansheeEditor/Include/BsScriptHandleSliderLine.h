#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptHandleSlider.h"
#include "BsHandleSliderLine.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for HandleSliderLine.
	 */
	class BS_SCR_BED_EXPORT ScriptHandleSliderLine : public ScriptObject <ScriptHandleSliderLine, ScriptHandleSliderBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "HandleSliderLine")

	protected:
		/**
		 * @copydoc	ScriptHandleSliderBase::getSlider
		 */
		virtual HandleSlider* getSlider() const override { return mSlider; }

		/**
		 * @copydoc	ScriptHandleSliderBase::getSlider
		 */
		virtual void destroyInternal() override;

	private:
		ScriptHandleSliderLine(MonoObject* instance, const Vector3& direction, float length, bool fixedScale, UINT64 layer);
		~ScriptHandleSliderLine();

		HandleSliderLine* mSlider;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, Vector3* direction, float length, bool fixedScale, UINT64 layer);
		static void internal_GetDelta(ScriptHandleSliderLine* nativeInstance, float* value);
	};
}