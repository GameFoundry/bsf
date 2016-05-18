//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsOAImporter.h"
#include "BsDataStream.h"
#include "BsFileSystem.h"
#include "BsOAWaveReader.h"
#include "BsOAFLACReader.h"
#include "BsOAOggVorbisReader.h"
#include "BsOAOggVorbisWriter.h"
#include "BsAudioClipImportOptions.h"

namespace BansheeEngine
{
	void convertToMono8(UINT8* input, UINT8* output, UINT32 numSamples, UINT32 numChannels)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			UINT16 sum = 0;
			for (UINT32 j = 0; j < numChannels; j++)
			{
				sum += *input;
				++input;
			}

			*output = sum / numChannels;
			++output;
		}
	}

	void convertToMono16(INT16* input, INT16* output, UINT32 numSamples, UINT32 numChannels)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			INT32 sum = 0;
			for (UINT32 j = 0; j < numChannels; j++)
			{
				sum += *input;
				++input;
			}

			*output = sum / numChannels;
			++output;
		}
	}

	void convertToMono24(UINT8* input, UINT8* output, UINT32 numSamples, UINT32 numChannels)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			INT32 sum = 0;
			for (UINT32 j = 0; j < numChannels; j++)
			{
				INT32 value = 0;
				bool isNegative = input[2] & 0x80;
				if (isNegative) // Sign extend if negative
					value = (0xFF << 24) | (input[2] << 16) | (input[1] << 8) | input[0];
				else
					value = (input[2] << 16) | (input[1] << 8) | input[0];

				sum += value;
				input += 3;
			}

			INT32 avg = sum / numChannels;
			output[0] = avg & 0x000000FF;
			output[1] = (avg >> 8) & 0x000000FF;
			output[2] = (avg >> 16) & 0x000000FF;
			output += 3;
		}
	}

	void convertToMono32(INT32* input, INT32* output, UINT32 numSamples, UINT32 numChannels)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			INT64 sum = 0;
			for (UINT32 j = 0; j < numChannels; j++)
			{
				sum += *input;
				++input;
			}

			*output = sum / numChannels;
			++output;
		}
	}

	OAImporter::OAImporter()
		:SpecificImporter()
	{

	}

	OAImporter::~OAImporter()
	{

	}

	bool OAImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return lowerCaseExt == L"wav" || lowerCaseExt == L"flac" || lowerCaseExt == L"ogg";
	}

	bool OAImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		// Don't check for magic number, rely on extension
		return true;
	}

	SPtr<Resource> OAImporter::import(const Path& filePath, SPtr<const ImportOptions> importOptions)
	{
		SPtr<DataStream> stream = FileSystem::openFile(filePath);

		WString extension = filePath.getWExtension();
		StringUtil::toLowerCase(extension);

		UPtr<OAFileReader> reader(nullptr, nullptr);
		if(extension == L"wav")
			reader = bs_unique_ptr<OAFileReader>(bs_new<OAWaveReader>());
		else if(extension == L"flac")
			reader = bs_unique_ptr<OAFileReader>(bs_new<OAFLACReader>());
		else if(extension == L"ogg")
			reader = bs_unique_ptr<OAFileReader>(bs_new<OAOggVorbisReader>());

		if (reader == nullptr)
			return nullptr;

		AudioFileInfo info;
		if (!reader->isValid(stream))
			return nullptr;

		if (!reader->open(stream, info))
			return nullptr;

		UINT32 bufferSize = info.numSamples * info.bitDepth;
		UINT8* sampleBuffer = (UINT8*)bs_alloc(bufferSize);
		reader->read(sampleBuffer, info.numSamples);

		SPtr<const AudioClipImportOptions> clipIO = std::static_pointer_cast<const AudioClipImportOptions>(importOptions);

		// If 3D, convert to mono
		if(clipIO->getIs3D() && info.numChannels > 1)
		{
			UINT32 numSamplesPerChannel = info.numSamples / info.numChannels;

			UINT32 monoBufferSize = numSamplesPerChannel * info.bitDepth;
			UINT8* monoBuffer = (UINT8*)bs_alloc(monoBufferSize);

			for(UINT32 i = 0; i < numSamplesPerChannel; i++)
			{
				switch(info.bitDepth)
				{
				case 8:
					convertToMono8(sampleBuffer, monoBuffer, numSamplesPerChannel, info.numChannels);
					break;
				case 16:
					convertToMono16((INT16*)sampleBuffer, (INT16*)monoBuffer, numSamplesPerChannel, info.numChannels);
					break;
				case 24:
					convertToMono24(sampleBuffer, monoBuffer, numSamplesPerChannel, info.numChannels);
					break;
				case 32:
					convertToMono32((INT32*)sampleBuffer, (INT32*)monoBuffer, numSamplesPerChannel, info.numChannels);
					break;
				default:
					assert(false);
					break;
				}

			}

			info.numSamples = numSamplesPerChannel;
			info.numChannels = 1;

			bs_free(sampleBuffer);

			sampleBuffer = monoBuffer;
			bufferSize = monoBufferSize;
		}

		// Encode to Ogg Vorbis if needed
		if(clipIO->getFormat() == AudioFormat::VORBIS)
		{
			// Note: If the original source was in Ogg Vorbis we could just copy it here, but instead we decode to PCM and
			// then re-encode which is redundant. If later we decide to copy be aware that the engine encodes Ogg in a
			// specific quality, and the the import source might have lower or higher bitrate/quality.
			UINT8* encodedSamples = OAOggVorbisWriter::PCMToOggVorbis(sampleBuffer, info, bufferSize);

			bs_free(sampleBuffer);
			sampleBuffer = encodedSamples;
		}

		SPtr<MemoryDataStream> sampleStream = bs_shared_ptr_new<MemoryDataStream>(sampleBuffer, bufferSize);

		AUDIO_CLIP_DESC clipDesc;
		clipDesc.bitDepth = info.bitDepth;
		clipDesc.format = clipIO->getFormat();
		clipDesc.frequency = info.sampleRate;
		clipDesc.numChannels = info.numChannels;
		clipDesc.readMode = clipIO->getReadMode();
		clipDesc.is3D = clipIO->getIs3D();

		SPtr<AudioClip> clip = AudioClip::_createPtr(sampleStream, bufferSize, info.numSamples, clipDesc);

		WString fileName = filePath.getWFilename(false);
		clip->setName(fileName);

		return clip;
	}
}