//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsVector3.h"

namespace BansheeEngine
{
	/** @addtogroup Audio
	 *  @{
	 */

	class BS_CORE_EXPORT AudioListener
	{
	public:
		virtual ~AudioListener() {}

		virtual void setPosition(const Vector3& position);
		Vector3 getPosition() const { return mPosition; }

		virtual void setDirection(const Vector3& direction);
		Vector3 getDirection() const { return mDirection; }

		virtual void setUp(const Vector3& up);
		Vector3 getUp() const { return mUp; }

		virtual void setVelocity(const Vector3& velocity);
		Vector3 getVelocity() const { return mVelocity; }

		static SPtr<AudioListener> create();

	protected:
		AudioListener();

		Vector3 mPosition;
		Vector3 mDirection;
		Vector3 mUp;
		Vector3 mVelocity;
	};

	/** @} */
}