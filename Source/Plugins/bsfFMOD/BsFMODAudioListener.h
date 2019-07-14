//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsFMODPrerequisites.h"
#include "Audio/BsAudioListener.h"

namespace bs
{
	/** @addtogroup FMOD
	 *  @{
	 */
	
	/** FMOD implementation of an AudioListener. */
	class FMODAudioListener : public AudioListener
	{
	public:
		FMODAudioListener();
		virtual ~FMODAudioListener();

		/** @copydoc SceneActor::setTransform */
		void setTransform(const Transform& transform) override;

		/** @copydoc AudioListener::setVelocity */
		void setVelocity(const Vector3& velocity) override;

	private:
		friend class FMODAudio;

		/** Called by the FMODAudio system when the listener list changes. */
		void rebuild(INT32 id);

		INT32 mId;
	};

	/** @} */
}
