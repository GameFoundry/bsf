//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/** @addtogroup Audio
	 *  @{
	 */

	class BS_CORE_EXPORT Audio : public Module<Audio>
	{
	protected:
		friend class AudioClip;

		virtual SPtr<AudioClip> createClip(const SPtr<DataStream>& samples, UINT32 numSamples, 
			const AUDIO_CLIP_DESC& desc) = 0;
	};

	/** Provides easier access to Audio. */
	BS_CORE_EXPORT Audio& gAudio();

	/** @} */
}