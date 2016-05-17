//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAAudioListener.h"

namespace BansheeEngine
{
	OAAudioListener::OAAudioListener()
	{
		
	}

	OAAudioListener::~OAAudioListener()
	{
		
	}

	void OAAudioListener::setPosition(const Vector3& position)
	{
		AudioListener::setPosition(position);

		// TODO
	}

	void OAAudioListener::setDirection(const Vector3& direction)
	{
		AudioListener::setDirection(direction);

		// TODO
	}

	void OAAudioListener::setUp(const Vector3& up)
	{
		AudioListener::setUp(up);

		// TODO
	}

	void OAAudioListener::setVelocity(const Vector3& velocity)
	{
		AudioListener::setVelocity(velocity);

		// TODO
	}
}