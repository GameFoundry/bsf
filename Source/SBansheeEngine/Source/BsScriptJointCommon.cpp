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
		D6Joint::Drive output;
		MonoUtil::invokeThunk(getNativeDataThunk, instance, &output);
		return output;
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
		HingeJoint::Drive output;
		MonoUtil::invokeThunk(getNativeDataThunk, instance, &output);
		return output;
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
		LimitLinearRange output;
		MonoUtil::invokeThunk(getNativeDataThunk, instance, &output);
		return output;
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
		LimitLinear output;
		MonoUtil::invokeThunk(getNativeDataThunk, instance, &output);
		return output;
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
		LimitAngularRange output;
		MonoUtil::invokeThunk(getNativeDataThunk, instance, &output);
		return output;
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
		LimitConeRange output;
		MonoUtil::invokeThunk(getNativeDataThunk, instance, &output);
		return output;
	}
}