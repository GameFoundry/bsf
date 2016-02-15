//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsJoint.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT HingeJoint : public Joint
	{
	public:
		enum class Flag
		{
			Limit = 0x1,
			Drive = 0x2
		};

		struct Drive
		{
			float speed = 0.0f;
			float forceLimit = FLT_MAX;
			float gearRatio = 1.0f;
			bool freeSpin = false;
		};

	public:
		virtual ~HingeJoint() { }

		virtual Radian getAngle() const = 0;
		virtual float getSpeed() const = 0;

		virtual LimitAngular getLimit() const = 0;
		virtual void setLimit(const LimitAngular& limit) const = 0;

		virtual Drive getDrive() const = 0;
		virtual void setDrive(const Drive& drive) const = 0;

		virtual void setFlag(Flag flag, bool enabled) = 0;
		virtual bool hasFlag(Flag flag) = 0;

		static SPtr<HingeJoint> create();
	};
}