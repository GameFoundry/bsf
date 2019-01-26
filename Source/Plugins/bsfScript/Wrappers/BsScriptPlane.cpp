//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "Wrappers/BsScriptPlane.h"
#include "BsMonoManager.h"
#include "BsMonoClass.h"
#include "BsMonoUtil.h"

namespace bs
{
	ScriptPlane::ScriptPlane(MonoObject* instance)
		:ScriptObject(instance)
	{ }

	void ScriptPlane::initRuntimeData()
	{ }

	MonoObject* ScriptPlane::box(const Plane& value)
	{
		// We're casting away const but it's fine since structs are passed by value anyway
		return MonoUtil::box(metaData.scriptClass->_getInternalClass(), (void*)&value);
	}

	Plane ScriptPlane::unbox(MonoObject* obj)
	{
		return *(Plane*)MonoUtil::unbox(obj);
	}
}