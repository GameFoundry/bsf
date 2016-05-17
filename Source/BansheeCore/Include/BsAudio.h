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

	struct AudioDevice
	{
		WString name;
	};

	/** Provides global functionality relating to sounds and music. */
	class BS_CORE_EXPORT Audio : public Module<Audio>
	{
	public:
		virtual ~Audio() { }

		virtual void setVolume(float volume) = 0;
		virtual float getVolume() const = 0;

		virtual void setPaused(bool paused) = 0;
		virtual bool isPaused() const = 0;

		virtual void update() = 0;

		virtual void setActiveDevice(const AudioDevice& device) = 0;
		virtual AudioDevice getActiveDevice() const = 0;
		
		virtual AudioDevice getDefaultDevice() const = 0;
		virtual Vector<AudioDevice> getAllDevices() const = 0;

	protected:
		friend class AudioClip;
		friend class AudioListener;
		friend class AudioSource;

		/** 
		 * Creates a new audio clip.
		 *
		 * @param[in]	samples		Stream containing audio samples in format specified in @p desc.
		 * @param[in]	streamSize	Size of the audio data in the provided stream, in bytes.
		 * @param[in]	numSamples	Number of samples in @p samples stream.
		 * @param[in]	desc		Descriptor describing the type of the audio stream (format, sample rate, etc.).
		 * @return					Newly created AudioClip. Must be manually initialized.
		 */
		virtual SPtr<AudioClip> createClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
			const AUDIO_CLIP_DESC& desc) = 0;

		virtual SPtr<AudioListener> createListener() = 0;

		virtual SPtr<AudioSource> createSource() = 0;
	};

	/** Provides easier access to Audio. */
	BS_CORE_EXPORT Audio& gAudio();

	/** @} */
}