//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsJoint.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT DistanceJoint : public Joint
	{
	public:
		enum class Flag
		{
			MinDistance = 0x1,
			MaxDistance = 0x2,
			Spring = 0x4
		};

	public:
		virtual ~DistanceJoint() { }

		virtual float getDistance() const = 0;

		virtual float getMinDistance() const = 0;
		virtual void setMinDistance(float value) = 0;

		virtual float getMaxDistance() const = 0;
		virtual void setMaxDistance(float value) = 0;

		virtual float getTolerance() const = 0;
		virtual void setTolerance(float value) = 0;

		virtual Spring getSpring() const = 0;
		virtual void setSpring(const Spring& value) = 0;

		virtual void setFlag(Flag flag, bool enabled) = 0;
		virtual bool hasFlag(Flag flag) const = 0;

		static SPtr<DistanceJoint> create();
	};
}