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

	INT32 convert24To32Bits(const UINT8* input)
	{
		bool isNegative = input[2] & 0x80;
		if (isNegative) // Sign extend if negative
			return (0xFF << 24) | (input[2] << 16) | (input[1] << 8) | input[0];
		else
			return (input[2] << 16) | (input[1] << 8) | input[0];
	}

	void convert32To24Bits(INT32 input, UINT8* output)
	{
		UINT32 valToEncode = *(UINT32*)&input;
		output[0] = valToEncode & 0x000000FF;
		output[1] = (valToEncode >> 8) & 0x000000FF;
		output[2] = (valToEncode >> 16) & 0x000000FF;
	}

	void convertToMono24(UINT8* input, UINT8* output, UINT32 numSamples, UINT32 numChannels)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			INT32 sum = 0;
			for (UINT32 j = 0; j < numChannels; j++)
			{
				sum += convert24To32Bits(input);
				input += 3;
			}

			INT32 avg = sum / numChannels;
			convert32To24Bits(avg, output);
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

	void convert8To32Bits(UINT8* input, INT32* output, UINT32 numSamples)
	{
		for(UINT32 i = 0; i < numSamples; i++)
		{
			INT8 val = (INT8)(input[i] - 128);
			output[i] = val << 24;
		}
	}

	void convert16To32Bits(INT16* input, INT32* output, UINT32 numSamples)
	{
		for (UINT32 i = 0; i < numSamples; i++)
			output[i] = input[i] << 16;
	}

	void convert24To32Bits(UINT8* input, INT32* output, UINT32 numSamples)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			output[i] = convert24To32Bits(input);
			input += 3;
		}
	}

	void convert32To8Bits(INT32* input, UINT8* output, UINT32 numSamples)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			INT8 val = (INT8)(input[i] >> 24);
			output[i] = (UINT8)(val + 128);
		}
	}

	void convert32To16Bits(INT32* input, INT16* output, UINT32 numSamples)
	{
		for (UINT32 i = 0; i < numSamples; i++)
			output[i] = (INT16)(input[i] >> 16);
	}

	void convert32To24Bits(INT32* input, UINT8* output, UINT32 numSamples)
	{
		for (UINT32 i = 0; i < numSamples; i++)
		{
			convert32To24Bits(input[i], output);
			output += 3;
		}
	}

	void convertBitDepth(UINT8* input, UINT32 inBitDepth, UINT8* output, UINT32 outBitDepth, UINT32 numSamples)
	{
		INT32* srcBuffer = nullptr;

		bool needTempBuffer = inBitDepth != 32;
		if (needTempBuffer)
			srcBuffer = (INT32*)bs_stack_alloc(numSamples * sizeof(INT32));
		else
			srcBuffer = (INT32*)input;

		// Note: I convert to a temporary 32-bit buffer and then use that to convert to actual requested bit depth. 
		//       It would be more efficient to convert directly from source to requested depth without a temporary buffer,
		//       at the cost of additional complexity. If this method ever becomes a performance issue consider that.
		switch(inBitDepth)
		{
		case 8:
			convert8To32Bits(input, srcBuffer, numSamples);
			break;
		case 16:
			convert16To32Bits((INT16*)input, srcBuffer, numSamples);
			break;
		case 24:
			convert8To32Bits(input, srcBuffer, numSamples);
			break;
		case 32:
			// Do nothing
			break;
		default:
			assert(false);
			break;
		}

		switch(outBitDepth)
		{
		case 8:
			convert32To8Bits(srcBuffer, output, numSamples);
			break;
		case 16:
			convert32To16Bits(srcBuffer, (INT16*)output, numSamples);
			break;
		case 24:
			convert32To24Bits(srcBuffer, output, numSamples);
			break;
		case 32:
			memcpy(output, srcBuffer, numSamples * sizeof(INT32));
			break;
		default:
			assert(false);
			break;
		}

		if (needTempBuffer)
		{
			bs_stack_free(srcBuffer);
			srcBuffer = nullptr;
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

		// Convert bit depth if needed
		if(clipIO->getBitDepth() != info.bitDepth)
		{
			UINT32 outBufferSize = info.numSamples * (clipIO->getBitDepth() / 8);
			UINT8* outBuffer = (UINT8*)bs_alloc(outBufferSize);

			convertBitDepth(sampleBuffer, info.bitDepth, outBuffer, clipIO->getBitDepth(), info.numSamples);

			info.bitDepth = clipIO->getBitDepth();

			bs_free(sampleBuffer);

			sampleBuffer = outBuffer;
			bufferSize = outBufferSize;
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