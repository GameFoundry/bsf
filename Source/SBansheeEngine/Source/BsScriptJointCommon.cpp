//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptJointCommon.h"
#include "BsMonoClass.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"

namespace BansheeEngine
{
	ScriptD6JointDrive::GetNativeDataThunkDef ScriptD6JointDrive::getNativeDataThunk = nullptr;

	ScriptD6JointDrive::ScriptD6JointDrive(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptD6JointDrive::initRuntimeData()
	{
		getNativeDataThunk = (GetNativeDataThunkDef)metaData.scriptClass->getMethod("Internal_GetNative", 1)->getThunk();
	}

	D6Joint::Drive ScriptD6JointDrive::convert(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		MonoObject* boxedOutput = getNativeDataThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
		return *(D6Joint::Drive*)mono_object_unbox(boxedOutput);
	}

	ScriptHingeJointDrive::GetNativeDataThunkDef ScriptHingeJointDrive::getNativeDataThunk = nullptr;

	ScriptHingeJointDrive::ScriptHingeJointDrive(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptHingeJointDrive::initRuntimeData()
	{
		getNativeDataThunk = (GetNativeDataThunkDef)metaData.scriptClass->getMethod("Internal_GetNative", 1)->getThunk();
	}

	HingeJoint::Drive ScriptHingeJointDrive::convert(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		MonoObject* boxedOutput = getNativeDataThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
		return *(HingeJoint::Drive*)mono_object_unbox(boxedOutput);
	}

	ScriptLimitLinearRange::GetNativeDataThunkDef ScriptLimitLinearRange::getNativeDataThunk = nullptr;

	ScriptLimitLinearRange::ScriptLimitLinearRange(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptLimitLinearRange::initRuntimeData()
	{
		getNativeDataThunk = (GetNativeDataThunkDef)metaData.scriptClass->getMethod("Internal_GetNative", 1)->getThunk();
	}

	LimitLinearRange ScriptLimitLinearRange::convert(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		MonoObject* boxedOutput = getNativeDataThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
		return *(LimitLinearRange*)mono_object_unbox(boxedOutput);
	}

	ScriptLimitLinear::GetNativeDataThunkDef ScriptLimitLinear::getNativeDataThunk = nullptr;

	ScriptLimitLinear::ScriptLimitLinear(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptLimitLinear::initRuntimeData()
	{
		getNativeDataThunk = (GetNativeDataThunkDef)metaData.scriptClass->getMethod("Internal_GetNative", 1)->getThunk();
	}

	LimitLinear ScriptLimitLinear::convert(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		MonoObject* boxedOutput = getNativeDataThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
		return *(LimitLinear*)mono_object_unbox(boxedOutput);
	}

	ScriptLimitAngularRange::GetNativeDataThunkDef ScriptLimitAngularRange::getNativeDataThunk = nullptr;

	ScriptLimitAngularRange::ScriptLimitAngularRange(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptLimitAngularRange::initRuntimeData()
	{
		getNativeDataThunk = (GetNativeDataThunkDef)metaData.scriptClass->getMethod("Internal_GetNative", 1)->getThunk();
	}

	LimitAngularRange ScriptLimitAngularRange::convert(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		MonoObject* boxedOutput = getNativeDataThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
		return *(LimitAngularRange*)mono_object_unbox(boxedOutput);
	}

	ScriptLimitConeRange::GetNativeDataThunkDef ScriptLimitConeRange::getNativeDataThunk = nullptr;

	ScriptLimitConeRange::ScriptLimitConeRange(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptLimitConeRange::initRuntimeData()
	{
		getNativeDataThunk = (GetNativeDataThunkDef)metaData.scriptClass->getMethod("Internal_GetNative", 1)->getThunk();
	}

	LimitConeRange ScriptLimitConeRange::convert(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		MonoObject* boxedOutput = getNativeDataThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
		return *(LimitConeRange*)mono_object_unbox(boxedOutput);
	}
}