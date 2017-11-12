//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2017 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptHandleSliderSphere.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"

namespace bs
{
	ScriptHandleSliderSphere::ScriptHandleSliderSphere(MonoObject* instance, float radius, bool fixedScale, UINT64 layer)
		:ScriptObject(instance), mSlider(nullptr)
	{
		mSlider = bs_new<HandleSliderSphere>(radius, fixedScale, layer);
	}

	ScriptHandleSliderSphere::~ScriptHandleSliderSphere()
	{
		if (mSlider != nullptr)
			bs_delete(mSlider);
	}

	void ScriptHandleSliderSphere::destroyInternal()
	{
		if (mSlider != nullptr)
			bs_delete(mSlider);

		mSlider = nullptr;
	}

	void ScriptHandleSliderSphere::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", (void*)&ScriptHandleSliderSphere::internal_CreateInstance);
	}

	void ScriptHandleSliderSphere::internal_CreateInstance(MonoObject* instance, float radius, bool fixedScale, UINT64 layer)
	{
		new (bs_alloc<ScriptHandleSliderSphere>()) ScriptHandleSliderSphere(instance, radius, fixedScale, layer);
	}
}
