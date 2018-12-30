//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Importer/BsImportOptions.h"
#include "Audio/BsAudioClip.h"

namespace bs
{
	/** @addtogroup Importer
	 *  @{
	 */

	/** Contains import options you may use to control how an audio clip is imported. */
	class BS_CORE_EXPORT AudioClipImportOptions : public ImportOptions
	{
	public:
		AudioClipImportOptions() = default;

		/** Returns the audio format to import the audio clip as. */
		AudioFormat getFormat() const { return mFormat; }

		/** Sets the audio format to import the audio clip as. */
		void setFormat(AudioFormat format) { mFormat = format; }

		/** Returns read mode that determines how is audio data loaded into memory. */
		AudioReadMode getReadMode() const { return mReadMode; }

		/** Sets read mode that determines how is audio data loaded into memory. */
		void setReadMode(AudioReadMode readMode) { mReadMode = readMode; }

		/** Checks should the clip be played as spatial (3D) audio or as normal audio. */
		bool getIs3D() const { return mIs3D; }

		/** 
		 * Sets whether the clip should be played as spatial (3D) audio or as normal audio. 3D clips will be converted
		 * to mono on import.
		 */
		void setIs3D(bool is3d) { mIs3D = is3d; }

		/** Returns the size of a single sample in bits. */
		UINT32 getBitDepth() const { return mBitDepth; }

		/** Sets the size of a single sample in bits. The clip will be converted to this bit depth on import. */
		void setBitDepth(UINT32 bitDepth) { mBitDepth = bitDepth; }

		// Note: Add options to resample to a different frequency

		/** Creates a new import options object that allows you to customize how are audio clips imported. */
		static SPtr<AudioClipImportOptions> create();

	private:
		AudioFormat mFormat = AudioFormat::PCM;
		AudioReadMode mReadMode = AudioReadMode::LoadDecompressed;
		bool mIs3D = true;
		UINT32 mBitDepth = 16;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class AudioClipImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;
	};

	/** @} */
}
