#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "BsScriptHandleSlider.h"
#include "BsHandleSliderPlane.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interop class between C++ & CLR for HandleSliderPlane.
	 */
	class BS_SCR_BED_EXPORT ScriptHandleSliderPlane : public ScriptObject <ScriptHandleSliderPlane, ScriptHandleSliderBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "HandleSliderPlane")

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
		ScriptHandleSliderPlane(MonoObject* instance, const Vector3& dir1, const Vector3& dir2, float length, 
			bool fixedScale, UINT64 layer);
		~ScriptHandleSliderPlane();

		HandleSliderPlane* mSlider;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, Vector3* dir1, Vector3* dir2, float length, 
			bool fixedScale, UINT64 layer);
		static void internal_GetDelta(ScriptHandleSliderPlane* nativeInstance, Vector2* value);
	};
}