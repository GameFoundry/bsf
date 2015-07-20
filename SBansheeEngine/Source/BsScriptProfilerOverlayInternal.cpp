#include "BsScriptProfilerOverlayInternal.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoUtil.h"
#include "BsApplication.h"
#include "BsCameraHandler.h"
#include "BsScriptCameraHandler.h"

namespace BansheeEngine
{
	ScriptProfilerOverlayInternal::ScriptProfilerOverlayInternal(MonoObject* managedInstance, const SPtr<CameraHandler>& camera)
		:ScriptObject(managedInstance), mProfilerOverlayInternal(nullptr)
	{
		if (camera != nullptr)
			mProfilerOverlayInternal = bs_new<ProfilerOverlayInternal>(camera->getViewport());
	}

	ScriptProfilerOverlayInternal::~ScriptProfilerOverlayInternal()
	{
		if (mProfilerOverlayInternal != nullptr)
			bs_delete(mProfilerOverlayInternal);
	}

	void ScriptProfilerOverlayInternal::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptProfilerOverlayInternal::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_SetType", &ScriptProfilerOverlayInternal::internal_SetType);
		metaData.scriptClass->addInternalCall("Internal_Update", &ScriptProfilerOverlayInternal::internal_Update);
		metaData.scriptClass->addInternalCall("Internal_DestroyInstance", &ScriptProfilerOverlayInternal::internal_DestroyInstance);
	}

	void ScriptProfilerOverlayInternal::internal_CreateInstance(MonoObject* instance, ScriptCameraHandler* camera)
	{
		SPtr<CameraHandler> cameraHandler;
		if (camera != nullptr)
			cameraHandler = camera->getInternal();

		ScriptProfilerOverlayInternal* nativeInstance = new (bs_alloc<ScriptProfilerOverlayInternal>()) ScriptProfilerOverlayInternal(instance, cameraHandler);
	}

	void ScriptProfilerOverlayInternal::internal_SetType(ScriptProfilerOverlayInternal* thisPtr, ProfilerOverlayType type)
	{
		if (thisPtr->mProfilerOverlayInternal != nullptr)
			thisPtr->mProfilerOverlayInternal->show(type);
	}

	void ScriptProfilerOverlayInternal::internal_Update(ScriptProfilerOverlayInternal* thisPtr)
	{
		if (thisPtr->mProfilerOverlayInternal != nullptr)
			thisPtr->mProfilerOverlayInternal->update();
	}

	void ScriptProfilerOverlayInternal::internal_DestroyInstance(ScriptProfilerOverlayInternal* thisPtr)
	{
		if (thisPtr->mProfilerOverlayInternal != nullptr)
		{
			bs_delete(thisPtr->mProfilerOverlayInternal);
			thisPtr->mProfilerOverlayInternal = nullptr;
		}
	}
}