//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsNullAudioPrerequisites.h"
#include "Audio/BsAudio.h"
#include "Audio/BsAudioClip.h"
#include "Audio/BsAudioListener.h"
#include "Audio/BsAudioSource.h"

namespace bs
{
	/** @addtogroup NullAudio
	 *  @{
	 */
	
	/** Global manager for the null audio implementation. */
	class NullAudio final : public Audio
	{
	public:
		NullAudio();

		/** @copydoc Audio::setVolume */
		void setVolume(float volume) override { mVolume = volume; }

		/** @copydoc Audio::getVolume */
		float getVolume() const override { return mVolume; }

		/** @copydoc Audio::setPaused */
		void setPaused(bool paused) override { mIsPaused = paused; }

		/** @copydoc Audio::isPaused */
		bool isPaused() const override { return mIsPaused; }

		/** @copydoc Audio::setActiveDevice */
		void setActiveDevice(const AudioDevice& device) override { mActiveDevice = device; }

		/** @copydoc Audio::getActiveDevice */
		AudioDevice getActiveDevice() const override { return mActiveDevice; }

		/** @copydoc Audio::getDefaultDevice */
		AudioDevice getDefaultDevice() const override { return mDefaultDevice; }

		/** @copydoc Audio::getAllDevices */
		const Vector<AudioDevice>& getAllDevices() const override { return mAllDevices; };

	private:
		friend class NullAudioSource;

		/** @copydoc Audio::createClip */
		SPtr<AudioClip> createClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
			const AUDIO_CLIP_DESC& desc) override;

		/** @copydoc Audio::createListener */
		SPtr<AudioListener> createListener() override;

		/** @copydoc Audio::createSource */
		SPtr<AudioSource> createSource() override;

		float mVolume = 1.0f;
		bool mIsPaused = false;

		Vector<AudioDevice> mAllDevices;
		AudioDevice mDefaultDevice;
		AudioDevice mActiveDevice;
	};

	/** Null implementation of an AudioClip. */
	class NullAudioClip final : public AudioClip
	{
	public:
		NullAudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);

	protected:
		/** @copydoc Resource::initialize */
		void initialize() override;

		/** @copydoc AudioClip::getSourceStream */
		SPtr<DataStream> getSourceStream(UINT32& size) override;

	private:
		// These streams exist to save original audio data in case it's needed later (usually for saving with the editor, or
		// manual data manipulation). In normal usage (in-game) these will be null so no memory is wasted.
		SPtr<DataStream> mSourceStreamData;
		UINT32 mSourceStreamSize;
	};

	/** Null implementation of an AudioListener. */
	class NullAudioListener final : public AudioListener
	{
	private:
		friend class NullAudio;
	};

	/** Null implementation of an AudioSource. */
	class NullAudioSource final : public AudioSource
	{
	public:
		/** @copydoc AudioSource::setTime */
		void setTime(float time) override { mTime = time; }

		/** @copydoc AudioSource::getTime */
		float getTime() const override { return mTime;}

		/** @copydoc AudioSource::play */
		void play() override { mState = AudioSourceState::Playing; }

		/** @copydoc AudioSource::pause */
		void pause() override { mState = AudioSourceState::Paused; }

		/** @copydoc AudioSource::stop */
		void stop() override { mState = AudioSourceState::Stopped; }

		/** @copydoc AudioSource::getState */
		AudioSourceState getState() const override { return mState; }

	private:
		friend class NullAudio;

		float mTime = 0.0f;
		AudioSourceState mState = AudioSourceState::Stopped;
		bool mGloballyPaused = false;
	};

	/** Provides easier access to the null audio manager. */
	NullAudio& gNullAudio();

	/** @} */
}
