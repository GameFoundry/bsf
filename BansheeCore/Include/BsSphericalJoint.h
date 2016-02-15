//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsJoint.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT SphericalJoint : public Joint
	{
	public:
		enum class Flag
		{
			Limit = 0x1
		};

	public:
		virtual ~SphericalJoint() { }

		virtual LimitConeRange getLimit() const = 0;
		virtual void setLimit(const LimitConeRange& limit) = 0;

		virtual void setFlag(Flag flag, bool enabled) = 0;
		virtual bool hasFlag(Flag flag) = 0;

		static SPtr<SphericalJoint> create();
	};
}