//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptRenderTarget.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "RenderAPI/BsRenderTarget.h"
#include "CoreThread/BsCoreThread.h"

namespace bs
{
	ScriptRenderTarget::ScriptRenderTarget(MonoObject* instance)
		:ScriptObject(instance)
	{

	}

	void ScriptRenderTarget::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetWidth", (void*)&ScriptRenderTarget::internal_getWidth);
		metaData.scriptClass->addInternalCall("Internal_GetHeight", (void*)&ScriptRenderTarget::internal_getHeight);
		metaData.scriptClass->addInternalCall("Internal_GetGammaCorrection", (void*)&ScriptRenderTarget::internal_getGammaCorrection);
		metaData.scriptClass->addInternalCall("Internal_GetSampleCount", (void*)&ScriptRenderTarget::internal_getSampleCount);
		metaData.scriptClass->addInternalCall("Internal_GetPriority", (void*)&ScriptRenderTarget::internal_getPriority);
		metaData.scriptClass->addInternalCall("Internal_SetPriority", (void*)&ScriptRenderTarget::internal_setPriority);
	}

	void ScriptRenderTarget::internal_getWidth(ScriptRenderTarget* thisPtr, int* value)
	{
		SPtr<RenderTarget> rt = thisPtr->getNativeValue();
		*value = rt->getProperties().width;
	}

	void ScriptRenderTarget::internal_getHeight(ScriptRenderTarget* thisPtr, int* value)
	{
		SPtr<RenderTarget> rt = thisPtr->getNativeValue();
		*value = rt->getProperties().height;
	}

	void ScriptRenderTarget::internal_getGammaCorrection(ScriptRenderTarget* thisPtr, bool* value)
	{
		SPtr<RenderTarget> rt = thisPtr->getNativeValue();
		*value = rt->getProperties().hwGamma;
	}

	void ScriptRenderTarget::internal_getSampleCount(ScriptRenderTarget* thisPtr, int* value)
	{
		SPtr<RenderTarget> rt = thisPtr->getNativeValue();
		*value = rt->getProperties().multisampleCount;
	}

	void ScriptRenderTarget::internal_getPriority(ScriptRenderTarget* thisPtr, int* value)
	{
		SPtr<RenderTarget> rt = thisPtr->getNativeValue();
		*value = rt->getProperties().priority;
	}

	void ScriptRenderTarget::internal_setPriority(ScriptRenderTarget* thisPtr, int value)
	{
		SPtr<RenderTarget> rt = thisPtr->getNativeValue();
		rt->setPriority(value);
	}
}