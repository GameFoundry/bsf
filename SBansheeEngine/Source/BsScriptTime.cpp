#include "BsScriptTime.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsTime.h"

namespace BansheeEngine
{
	ScriptTime::ScriptTime(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptTime::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_GetElapsed", &ScriptTime::internal_getElapsed);
		metaData.scriptClass->addInternalCall("Internal_GetFrameDelta", &ScriptTime::internal_getFrameDelta);
		metaData.scriptClass->addInternalCall("Internal_GetFrameNumber", &ScriptTime::internal_getFrameNumber);
		metaData.scriptClass->addInternalCall("Internal_GetPrecise", &ScriptTime::internal_getPrecise);
	}

	float ScriptTime::internal_getElapsed()
	{
		return gTime().getTime();
	}

	float ScriptTime::internal_getFrameDelta()
	{
		return gTime().getFrameDelta();
	}

	UINT32 ScriptTime::internal_getFrameNumber()
	{
		return gTime().getFrameNumber();
	}

	UINT64 ScriptTime::internal_getPrecise()
	{
		return gTime().getTimePrecise();
	}
}