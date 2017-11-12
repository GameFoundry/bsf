//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2017 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsScriptObject.h"
#include "Wrappers/BsScriptHandleSlider.h"
#include "Handles/BsHandleSliderSphere.h"

namespace bs
{
	/** @addtogroup ScriptInteropEditor
	 *  @{
	 */

	/**	Interop class between C++ & CLR for HandleSliderSphere. */
	class BS_SCR_BED_EXPORT ScriptHandleSliderSphere : public ScriptObject <ScriptHandleSliderSphere, ScriptHandleSliderBase>
	{
	public:
		SCRIPT_OBJ(EDITOR_ASSEMBLY, "BansheeEditor", "HandleSliderSphere")

	protected:
		/** @copydoc ScriptHandleSliderBase::getSlider */
		HandleSlider* getSlider() const override { return mSlider; }

		/** @copydoc ScriptHandleSliderBase::getSlider */
		void destroyInternal() override;

	private:
		ScriptHandleSliderSphere(MonoObject* instance, float radius, bool fixedScale, UINT64 layer);
		~ScriptHandleSliderSphere();

		HandleSliderSphere* mSlider;

		/************************************************************************/
		/* 								CLR HOOKS						   		*/
		/************************************************************************/
		static void internal_CreateInstance(MonoObject* instance, float radius, bool fixedScale, UINT64 layer);
	};

	/** @} */
}
