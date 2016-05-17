//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsOAPrerequisites.h"
#include "BsAudioClip.h"
#include "BsOAOggVorbisReader.h"

namespace BansheeEngine
{
	/** @addtogroup OpenAudio
	 *  @{
	 */
	
	class BS_OA_EXPORT OAAudioClip : public AudioClip
	{
	public:
		/** @copydoc AudioClip::getSamples */
		void getSamples(UINT8* samples, UINT32 count) const override;

		/** @copydoc AudioClip::seekSamples */
		void seekSamples(UINT32 offset) override;

	protected:
		/** @copydoc Resource::initialize */
		void initialize() override;

		/** @copydoc AudioClip::getSourceFormatData */
		SPtr<DataStream> getSourceFormatData(UINT32& size) override;
	private:
		OAAudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc);

		mutable Mutex mMutex;
		mutable OAOggVorbisReader mVorbisReader;
		bool mNeedsDecompression;
	};

	/** @} */
}