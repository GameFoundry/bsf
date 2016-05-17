//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsOAPrerequisites.h"
#include "BsAudioListener.h"

namespace BansheeEngine
{
	/** @addtogroup OpenAudio
	 *  @{
	 */
	
	class BS_OA_EXPORT OAAudioListener : public AudioListener
	{
	public:
		OAAudioListener();
		virtual ~OAAudioListener();

		/** @copydoc AudioListener::setPosition */
		void setPosition(const Vector3& position) override;

		/** @copydoc AudioListener::setDirection */
		void setDirection(const Vector3& direction) override;

		/** @copydoc AudioListener::setUp */
		void setUp(const Vector3& up) override;

		/** @copydoc AudioListener::setVelocity */
		void setVelocity(const Vector3& velocity) override;
	};

	/** @} */
}