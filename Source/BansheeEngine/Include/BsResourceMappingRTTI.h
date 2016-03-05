//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsGameResourceManager.h"

namespace BansheeEngine
{
	class BS_EXPORT ResourceMappingRTTI : public RTTIType<ResourceMapping, IReflectable, ResourceMappingRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mMapping)

	public:
		ResourceMappingRTTI()
		{
			BS_ADD_PLAIN_FIELD(mMapping, 0);
		}

		const String& getRTTIName() override
		{
			static String name = "ResourceMapping";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_ResourceMapping;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return ResourceMapping::create();
		}
	};
}