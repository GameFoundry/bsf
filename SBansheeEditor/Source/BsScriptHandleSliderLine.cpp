#include "BsScriptHandleSliderLine.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	ScriptHandleSliderLine::ScriptHandleSliderLine(MonoObject* instance, const Vector3& direction, float length, bool fixedScale)
		:ScriptObject(instance), mSlider(nullptr)
	{
		mSlider = bs_new<HandleSliderLine>(direction, length, fixedScale);
	}

	ScriptHandleSliderLine::~ScriptHandleSliderLine()
	{
		if (mSlider != nullptr)
			bs_delete(mSlider);
	}

	void ScriptHandleSliderLine::destroyInternal()
	{
		if (mSlider != nullptr)
			bs_delete(mSlider);

		mSlider = nullptr;
	}

	void ScriptHandleSliderLine::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptHandleSliderLine::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetDelta", &ScriptHandleSliderLine::internal_GetDelta);
	}

	void ScriptHandleSliderLine::internal_CreateInstance(MonoObject* instance, Vector3 direction, float length, bool fixedScale)
	{
		ScriptHandleSliderLine* nativeInstance = new (bs_alloc<ScriptHandleSliderLine>()) 
			ScriptHandleSliderLine(instance, direction, length, fixedScale);
	}

	void ScriptHandleSliderLine::internal_GetDelta(ScriptHandleSliderLine* nativeInstance, float* value)
	{
		*value = nativeInstance->mSlider->getDelta();
	}
}