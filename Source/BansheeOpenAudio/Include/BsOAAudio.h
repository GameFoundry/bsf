//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsOAPrerequisites.h"
#include "BsAudio.h"
#include "AL/alc.h"

namespace BansheeEngine
{
	/** @addtogroup OpenAudio
	 *  @{
	 */
	
	/** Global manager for the audio implementation using OpenAL as the backend. */
	class BS_OA_EXPORT OAAudio : public Audio
	{
	public:
		OAAudio();
		virtual ~OAAudio();

		/** @copydoc Audio::setVolume */
		void setVolume(float volume) override;

		/** @copydoc Audio::getVolume */
		float getVolume() const override;

		/** @copydoc Audio::setPaused */
		void setPaused(bool paused) override;

		/** @copydoc Audio::isPaused */
		bool isPaused() const override;

		/** @copydoc Audio::update */
		void _update() override;

		/** @copydoc Audio::setActiveDevice */
		void setActiveDevice(const AudioDevice& device) override;

		/** @copydoc Audio::getActiveDevice */
		AudioDevice getActiveDevice() const override { return mActiveDevice; }

		/** @copydoc Audio::getDefaultDevice */
		AudioDevice getDefaultDevice() const override { return mDefaultDevice; }

		/** @copydoc Audio::getAllDevices */
		const Vector<AudioDevice>& getAllDevices() const override { return mAllDevices; };

		/** @name Internal 
		 *  @{
		 */

		/** Checks is a specific OpenAL extension supported. */
		bool _isExtensionSupported(const String& extension) const;

		/** Registers a new AudioListener. Should be called on listener creation. */
		void _registerListener(OAAudioListener* listener);

		/** Unregisters an existing AudioListener. Should be called before listener destruction. */
		void _unregisterListener(OAAudioListener* listener);

		/** Registers a new AudioSource. Should be called on source creation. */
		void _registerSource(OAAudioSource* source);

		/** Unregisters an existing AudioSource. Should be called before source destruction. */
		void _unregisterSource(OAAudioSource* source);

		/** Returns a list of all OpenAL contexts. Each listener has its own context. */
		const Vector<ALCcontext*>& _getContexts() const { return mContexts; }

		/** Returns an OpenAL context assigned to the provided listener. */
		ALCcontext* _getContext(const OAAudioListener* listener) const;

		/** @} */

	private:
		/** @copydoc Audio::createClip */
		SPtr<AudioClip> createClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples,
			const AUDIO_CLIP_DESC& desc) override;

		/** @copydoc Audio::createListener */
		SPtr<AudioListener> createListener() override;

		/** @copydoc Audio::createSource */
		SPtr<AudioSource> createSource() override;

		/** 
		 * Delete all existing contexts and rebuild them according to the listener list. All audio sources will be rebuilt
		 * as well.
		 *
		 * This should be called when listener count changes, or audio device is changed.
		 */
		void rebuildContexts();

		/** Delete all existing OpenAL contexts. */
		void clearContexts();

		float mVolume;

		ALCdevice* mDevice;
		Vector<AudioDevice> mAllDevices;
		AudioDevice mDefaultDevice;
		AudioDevice mActiveDevice;

		Vector<OAAudioListener*> mListeners;
		Vector<ALCcontext*> mContexts;
		UnorderedSet<OAAudioSource*> mSources;

	};

	/** Provides easier access to OAAudio. */
	OAAudio& gOAAudio();

	/** @} */
}