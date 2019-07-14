//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "Physics/BsHingeJoint.h"
#include "Physics/BsPhysics.h"

namespace bs
{
	SPtr<HingeJoint> HingeJoint::create(PhysicsScene& scene, const HINGE_JOINT_DESC& desc)
	{
		return scene.createHingeJoint(desc);
	}
}
