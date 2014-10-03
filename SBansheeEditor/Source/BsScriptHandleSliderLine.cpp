#include "BsScriptHandleSliderLine.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	ScriptHandleSliderLine::ScriptHandleSliderLine(MonoObject* instance, const Vector3& direction, float length, bool fixedScale, float snapValue)
		:ScriptObject(instance), mSlider(nullptr)
	{
		mSlider = bs_new<HandleSliderLine>(direction, length, fixedScale, snapValue);
	}

	ScriptHandleSliderLine::~ScriptHandleSliderLine()
	{
		bs_delete(mSlider);
	}

	void ScriptHandleSliderLine::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("internal_CreateInstance", &ScriptHandleSliderLine::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("internal_GetDelta", &ScriptHandleSliderLine::internal_GetDelta);
		metaData.scriptClass->addInternalCall("internal_GetNewPosition", &ScriptHandleSliderLine::internal_GetNewPosition);
	}

	void ScriptHandleSliderLine::internal_CreateInstance(MonoObject* instance, Vector3 direction, float length, bool fixedScale, float snapValue)
	{
		ScriptHandleSliderLine* nativeInstance = new (bs_alloc<ScriptHandleSliderLine>()) 
			ScriptHandleSliderLine(instance, direction, length, fixedScale, snapValue);
	}

	void ScriptHandleSliderLine::internal_GetDelta(ScriptHandleSliderLine* nativeInstance, float* value)
	{
		*value = nativeInstance->mSlider->getDelta();
	}

	void ScriptHandleSliderLine::internal_GetNewPosition(ScriptHandleSliderLine* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->mSlider->getNewPosition();
	}
}