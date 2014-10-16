#include "BsScriptHandleSliderDisc.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"

namespace BansheeEngine
{
	ScriptHandleSliderDisc::ScriptHandleSliderDisc(MonoObject* instance, const Vector3& normal, float radius, bool fixedScale, float snapValue)
		:ScriptObject(instance), mSlider(nullptr)
	{
		mSlider = bs_new<HandleSliderDisc>(normal, radius, snapValue, fixedScale);
	}

	ScriptHandleSliderDisc::~ScriptHandleSliderDisc()
	{
		if (mSlider != nullptr)
			bs_delete(mSlider);
	}

	void ScriptHandleSliderDisc::destroy()
	{
		if (mSlider != nullptr)
			bs_delete(mSlider);

		mSlider = nullptr;
	}

	void ScriptHandleSliderDisc::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptHandleSliderDisc::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_GetDelta", &ScriptHandleSliderDisc::internal_GetDelta);
		metaData.scriptClass->addInternalCall("Internal_GetDeltaDirection", &ScriptHandleSliderDisc::internal_GetDeltaDirection);
		metaData.scriptClass->addInternalCall("Internal_GetNewRotation", &ScriptHandleSliderDisc::internal_GetNewRotation);
	}

	void ScriptHandleSliderDisc::internal_CreateInstance(MonoObject* instance, Vector3 normal, float radius, bool fixedScale, float snapValue)
	{
		ScriptHandleSliderDisc* nativeInstance = new (bs_alloc<ScriptHandleSliderDisc>())
			ScriptHandleSliderDisc(instance, normal, radius, fixedScale, snapValue);
	}

	void ScriptHandleSliderDisc::internal_GetDelta(ScriptHandleSliderDisc* nativeInstance, float* value)
	{
		*value = nativeInstance->mSlider->getDelta();
	}

	void ScriptHandleSliderDisc::internal_GetDeltaDirection(ScriptHandleSliderDisc* nativeInstance, Vector3* value)
	{
		*value = nativeInstance->mSlider->getDeltaDirection();
	}

	void ScriptHandleSliderDisc::internal_GetNewRotation(ScriptHandleSliderDisc* nativeInstance, Quaternion* value)
	{
		*value = nativeInstance->mSlider->getNewRotation();
	}
}