//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "foundation\PxVec3.h"
#include "foundation\PxVec4.h"
#include "foundation\PxQuat.h"
#include "foundation\PxTransform.h"

namespace BansheeEngine
{
#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(BS_STATIC_LIB)
#	ifdef BS_PHYSX_EXPORTS
#		define BS_PHYSX_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_PHYSX_EXPORT
#       else
#    		define BS_PHYSX_EXPORT __declspec(dllimport)
#       endif
#	endif
#else
#	define BS_PHYSX_EXPORT
#endif

	class PhysXRigidbody;
	class PhsyXMaterial;
	class FPhysXCollider;

	inline const physx::PxVec3& toPxVector(const Vector3& input)
	{
		return *(physx::PxVec3*)&input;
	}

	inline const physx::PxVec4& toPxVector(const Vector4& input)
	{
		return *(physx::PxVec4*)&input;
	}

	inline const physx::PxQuat& toPxQuaternion(const Quaternion& input)
	{
		return *(physx::PxQuat*)&input;
	}

	inline physx::PxTransform toPxTransform(const Vector3& pos, const Quaternion& rot)
	{
		return physx::PxTransform(toPxVector(pos), toPxQuaternion(rot));
	}

	inline const Vector3& fromPxVector(const physx::PxVec3& input)
	{
		return *(Vector3*)&input;
	}

	inline const Vector4& fromPxVector(const physx::PxVec4& input)
	{
		return *(Vector4*)&input;
	}

	inline const Quaternion& fromPxQuaternion(const physx::PxQuat& input)
	{
		return *(Quaternion*)&input;
	}
}