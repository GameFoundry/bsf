//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAAudioClip.h"
#include "BsOAOggVorbisWriter.h"
#include "BsOAOggVorbisReader.h"
#include "BsDataStream.h"

namespace BansheeEngine
{
	OAAudioClip::OAAudioClip(const SPtr<DataStream>& samples, UINT32 streamSize, UINT32 numSamples, const AUDIO_CLIP_DESC& desc)
		:AudioClip(samples, streamSize, numSamples, desc), mNeedsDecompression(false)
	{ }

	void OAAudioClip::initialize()
	{
		{
			Lock lock(mMutex); // Needs to be called even if stream data is null, to ensure memory fence is added so the
							   // other thread sees properly initialized AudioClip members

			AudioFileInfo info;
			info.bitDepth = mDesc.bitDepth;
			info.numChannels = mDesc.numChannels;
			info.numSamples = mNumSamples;
			info.sampleRate = mDesc.frequency;

			if (mDesc.readMode != AudioReadMode::Stream)
			{
				// If not streaming, but input stream is a file, read all of it into an internal memory stream
				if (mStreamData != nullptr)
				{
					// Decompress if needed
					if (mDesc.format == AudioFormat::VORBIS && mDesc.readMode == AudioReadMode::LoadDecompressed)
					{
						mStreamData->seek(mStreamOffset);

						OAOggVorbisReader reader;
						if (reader.open(mStreamData, info))
						{
							UINT32 bufferSize = info.numSamples * info.bitDepth;
							UINT8* sampleBuffer = (UINT8*)bs_alloc(bufferSize);

							reader.read(sampleBuffer, info.numSamples);

							mStreamData = bs_shared_ptr_new<MemoryDataStream>(sampleBuffer, bufferSize);
							mStreamOffset = 0;
							mStreamSize = bufferSize;
						}
						else
							LOGERR("Failed decompressing AudioClip stream.");
					}
					else
					{
						if (mStreamData->isFile()) // If reading from file, make a copy of data in memory
						{
							UINT8* data = (UINT8*)bs_alloc(mStreamSize);

							mStreamData->seek(mStreamOffset);
							mStreamData->read(data, mStreamSize);

							mStreamData = bs_shared_ptr_new<MemoryDataStream>(data, mStreamSize);
							mStreamOffset = 0;
						}
					}
				}
			}

			if (mDesc.format == AudioFormat::VORBIS && mDesc.readMode != AudioReadMode::LoadDecompressed)
			{
				mNeedsDecompression = true;

				if (mStreamData != nullptr)
				{
					if (!mVorbisReader.open(mStreamData, info))
						LOGERR("Failed decompressing AudioClip stream.");
				}
			}

			if (mStreamData != nullptr)
				mStreamData->seek(mStreamOffset);
		}

		AudioClip::initialize();
	}

	void OAAudioClip::getSamples(UINT8* samples, UINT32 count) const
	{
		Lock lock(mMutex);

		if (mStreamData == nullptr)
			return;

		if (mNeedsDecompression)
			mVorbisReader.read(samples, count);
		else
		{
			UINT32 bytesPerSample = mDesc.bitDepth / 8;
			UINT32 size = count * bytesPerSample;

			mStreamData->read(samples, size);
		}
	}

	void OAAudioClip::seekSamples(UINT32 offset)
	{
		Lock lock(mMutex);

		if (mStreamData == nullptr)
			return;

		if (mNeedsDecompression)
			mVorbisReader.seek(offset);
		else
		{
			UINT32 bytesPerSample = mDesc.bitDepth / 8;
			UINT32 streamOffset = mStreamOffset + offset * bytesPerSample;

			mStreamData->seek(streamOffset);
		}
	}

	SPtr<DataStream> OAAudioClip::getSourceFormatData(UINT32& size)
	{
		Lock lock(mMutex);

		if (mStreamData == nullptr)
		{
			size = 0;
			return nullptr;
		}

		if(mDesc.format == AudioFormat::PCM || mDesc.readMode == AudioReadMode::LoadCompressed || 
			mDesc.readMode == AudioReadMode::Stream)
		{
			SPtr<DataStream> outStream = mStreamData->clone(false);
			outStream->seek(mStreamOffset);

			size = mStreamSize;
			return outStream;
		}
		else // Data has been decompressed, we need to re-compress it to get the original stream
		{
			assert(!mStreamData->isFile());
			SPtr<MemoryDataStream> memStream = std::static_pointer_cast<MemoryDataStream>(mStreamData);

			AudioFileInfo info;
			info.bitDepth = mDesc.bitDepth;
			info.numChannels = mDesc.numChannels;
			info.numSamples = mNumSamples;
			info.sampleRate = mDesc.frequency;

			UINT32 orgOffset = (UINT32)memStream->tell();
			memStream->seek(mStreamOffset);

			UINT32 bufferSize = 0;
			UINT8* encodedSamples = OAOggVorbisWriter::PCMToOggVorbis(memStream->getCurrentPtr(), info, bufferSize);

			memStream->seek(orgOffset);

			size = bufferSize;
			return bs_shared_ptr_new<MemoryDataStream>(encodedSamples, bufferSize);
		}
	}
}