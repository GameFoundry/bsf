//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsResource.h"

namespace BansheeEngine
{
	/** @addtogroup Audio
	 *  @{
	 */

	enum class AudioFormat
	{
		PCM,
		VORBIS
	};

	enum class AudioReadMode
	{
		LoadDecompressed,
		LoadCompressed,
		Stream
	};

	struct AUDIO_CLIP_DESC
	{
		AudioReadMode readMode = AudioReadMode::LoadDecompressed;
		AudioFormat format = AudioFormat::PCM;
		UINT32 frequency = 44100;
		UINT32 bitDepth = 16;
		UINT32 numChannels = 2;
	};

	class BS_CORE_EXPORT AudioClip : public Resource
	{
	public:
		static HAudioClip create(UINT32 numSamples, const AUDIO_CLIP_DESC& desc);
		static HAudioClip create(const SPtr<DataStream>& samples, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);

		UINT32 getBitDepth() const { return mDesc.bitDepth; }
		UINT32 getFrequency() const { return mDesc.frequency; }
		UINT32 getNumChannels() const { return mDesc.numChannels; }
		AudioFormat getFormat() const { return mDesc.format; }
		AudioReadMode getReadMode() const { return mDesc.readMode; }
		UINT32 getLength() const { return mNumSamples / mDesc.frequency; }
		UINT32 getNumSamples() const { return mNumSamples; }

		virtual UINT32 getDataSize() const = 0;
		virtual UINT8* getData() const = 0;

		// Note: This will convert internal audio read mode to LoadCompressed (if ogg) or LoadDecompressed (if PCM)
		virtual void setData(UINT8* samples, UINT32 numSamples) = 0;

	public: // ***** INTERNAL ******
		/** @name Internal
		 *  @{
		 */

		static SPtr<AudioClip> _createPtr(const SPtr<DataStream>& samples, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);

		/** @} */
	private:
		AudioClip(const SPtr<DataStream>& samples, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);

	protected:
		AUDIO_CLIP_DESC mDesc;
		UINT32 mNumSamples;
		SPtr<DataStream> mStreamData;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class AudioClipRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}