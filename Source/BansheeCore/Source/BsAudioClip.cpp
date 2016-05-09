//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsAudioClip.h"
#include "BsResources.h"
#include "BsAudio.h"
#include "BsAudioClipRTTI.h"

namespace BansheeEngine
{
	AudioClip::AudioClip(const SPtr<DataStream>& samples, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
		:Resource(false), mDesc(desc), mNumSamples(numSamples), mStreamData(samples)
	{
		if (mDesc.readMode != AudioReadMode::Stream)
			mStreamData = nullptr;
	}

	HAudioClip AudioClip::create(UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
	{
		return static_resource_cast<AudioClip>(gResources()._createResourceHandle(_createPtr(nullptr, numSamples, desc)));
	}

	HAudioClip AudioClip::create(const SPtr<DataStream>& samples, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
	{
		return static_resource_cast<AudioClip>(gResources()._createResourceHandle(_createPtr(samples, numSamples, desc)));
	}

	SPtr<AudioClip> AudioClip::_createPtr(const SPtr<DataStream>& samples, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
	{
		SPtr<AudioClip> newClip = gAudio().createClip(samples, numSamples, desc);
		newClip->_setThisPtr(newClip);
		newClip->initialize();

		return newClip;
	}

	RTTITypeBase* AudioClip::getRTTIStatic()
	{
		return AudioClipRTTI::instance();
	}

	RTTITypeBase* AudioClip::getRTTI() const
	{
		return getRTTIStatic();
	}
}