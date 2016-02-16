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

			bool operator==(const Drive& other) const
			{
				return speed == other.speed && forceLimit == other.forceLimit && gearRatio == other.gearRatio && 
					freeSpin && other.freeSpin;
			}
		};

	public:
		virtual ~HingeJoint() { }

		virtual Radian getAngle() const = 0;
		virtual float getSpeed() const = 0;

		virtual LimitAngularRange getLimit() const = 0;
		virtual void setLimit(const LimitAngularRange& limit) = 0;

		virtual Drive getDrive() const = 0;
		virtual void setDrive(const Drive& drive) = 0;

		virtual void setFlag(Flag flag, bool enabled) = 0;
		virtual bool hasFlag(Flag flag) const = 0;

		static SPtr<HingeJoint> create();
	};
}