//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptFixedJoint.h"
#include "BsFixedJoint.h"

namespace BansheeEngine
{
	ScriptFixedJoint::ScriptFixedJoint(MonoObject* instance, const SPtr<Joint>& joint)
		:TScriptJoint(instance, joint)
	{

	}

	void ScriptFixedJoint::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptFixedJoint::internal_CreateInstance);
	}

	FixedJoint* ScriptFixedJoint::getFixedJoint() const
	{
		return static_cast<FixedJoint*>(mJoint.get());
	}

	void ScriptFixedJoint::internal_CreateInstance(MonoObject* instance)
	{
		SPtr<FixedJoint> joint = FixedJoint::create();
		joint->_setOwner(PhysicsOwnerType::Script, instance);

		ScriptFixedJoint* scriptJoint = new (bs_alloc<ScriptFixedJoint>()) ScriptFixedJoint(instance, joint);
	}
}