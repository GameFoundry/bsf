//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	class BS_CORE_EXPORT PhysicsMaterial : public Resource
	{
	public:
		virtual ~PhysicsMaterial() { }

		virtual void setStaticFriction(float value) = 0;
		virtual float getStaticFriction() const = 0;

		virtual void setDynamicFriction(float value) = 0;
		virtual float getDynamicFriction() const = 0;

		virtual void setRestitutionCoefficient(float value) = 0;
		virtual float getRestitutionCoefficient() const = 0;

		static HPhysicsMaterial create(float staticFriction = 0.0f, float dynamicFriction = 0.0f, float restitution = 0.0f);
		static PhysicsMaterialPtr _createPtr(float staticFriction = 0.0f, float dynamicFriction = 0.0f, float restitution = 0.0f);

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class PhysicsMaterialRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};
}