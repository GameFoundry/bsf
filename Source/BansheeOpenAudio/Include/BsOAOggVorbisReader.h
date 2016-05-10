//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsOAPrerequisites.h"
#include "vorbis\vorbisfile.h"

namespace BansheeEngine
{
	/** @addtogroup OpenAudio
	 *  @{
	 */

	/** Used for reading Ogg Vorbis audio files. */
	class OAOggVorbisReader
	{
	public:
		OAOggVorbisReader();
		~OAOggVorbisReader();

		bool open(const SPtr<DataStream>& stream, AudioFileInfo& info);
		void seek(UINT32 offset); // Offset in number of samples
		UINT32 read(UINT8* samples, UINT32 numSamples);

		static bool isValid(const SPtr<DataStream>& stream);
	private:
		SPtr<DataStream> mStream;
		OggVorbis_File mOggVorbisFile;
		UINT32 mChannelCount;
	};

	/** @} */
}