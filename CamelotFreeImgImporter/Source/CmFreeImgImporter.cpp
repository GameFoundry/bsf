#include "CmFreeImgImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"

#include "FreeImage.h"

// freeimage 3.9.1~3.11.0 interoperability fix
#ifndef FREEIMAGE_COLORORDER
// we have freeimage 3.9.1, define these symbols in such way as 3.9.1 really work (do not use 3.11.0 definition, as color order was changed between these two versions on Apple systems)
#define FREEIMAGE_COLORORDER_BGR	0
#define FREEIMAGE_COLORORDER_RGB	1
#if defined(FREEIMAGE_BIGENDIAN)
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_RGB
#else
#define FREEIMAGE_COLORORDER FREEIMAGE_COLORORDER_BGR
#endif
#endif

namespace CamelotEngine
{
	void FreeImageLoadErrorHandler(FREE_IMAGE_FORMAT fif, const char *message) 
	{
		// Callback method as required by FreeImage to report problems
		const char* typeName = FreeImage_GetFormatFromFIF(fif);
		if (typeName)
		{
			gDebug().log("FreeImage error: '" + String(message) + "' when loading format " + typeName, "AssetImport");
		}
		else
		{
			gDebug().log("FreeImage error: '" + String(message) + "'", "AssetImport");
		}
	}

	FreeImgImporter::FreeImgImporter()
		:SpecificImporter() 
	{
		FreeImage_Initialise(false);

		// Register codecs
		StringUtil::StrStreamType strExt;
		strExt << "Supported formats: ";
		bool first = true;
		for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
		{

			// Skip DDS codec since FreeImage does not have the option 
			// to keep DXT data compressed, we'll use our own codec
			if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
				continue;

			String exts(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i));
			if (!first)
			{
				strExt << ",";
			}
			first = false;
			strExt << exts;

			// Pull off individual formats (separated by comma by FI)
			vector<String>::type extsVector = StringUtil::split(exts, ",");
			for (auto v = extsVector.begin(); v != extsVector.end(); ++v)
			{
				auto findIter = std::find(mExtensions.begin(), mExtensions.end(), *v);

				if(findIter == mExtensions.end())
				{
					String ext = *v;
					StringUtil::toLowerCase(ext);

					mExtensionToFID.insert(std::make_pair(ext, i));
					mExtensions.push_back(ext);
				}
			}
		}

		// Set error handler
		FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
	}

	FreeImgImporter::~FreeImgImporter() 
	{
		FreeImage_DeInitialise();
	}

	bool FreeImgImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FreeImgImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		String ext = magicNumToExtension(magicNumPtr, numBytes);

		return isExtensionSupported(ext);
	}

	String FreeImgImporter::magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const
	{
		// Set error handler
		FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);

		FIMEMORY* fiMem = 
			FreeImage_OpenMemory((BYTE*)magic, static_cast<DWORD>(maxBytes));

		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMem, (int)maxBytes);
		FreeImage_CloseMemory(fiMem);

		if (fif != FIF_UNKNOWN)
		{
			String ext(FreeImage_GetFormatFromFIF(fif));
			StringUtil::toLowerCase(ext);
			return ext;
		}
		else
		{
			return StringUtil::BLANK;
		}
	}

	ResourcePtr FreeImgImporter::import(DataStream* fileData)
	{
		RawImageData imgData = importRawImage(fileData);
		if(imgData.data == nullptr)
			return nullptr;


		return nullptr;
	}

	FreeImgImporter::RawImageData FreeImgImporter::importRawImage(DataStream* fileData)
	{
		size_t magicLen = std::min(fileData->size(), (size_t)32);
		UINT8 magicBuf[32];
		fileData->read(magicBuf, magicLen);
		// return to start
		fileData->seek(0);

		String fileExtension = magicNumToExtension(magicBuf, magicLen);
		auto findFormat = mExtensionToFID.find(fileExtension);
		if(findFormat == mExtensionToFID.end())
		{
			CM_EXCEPT(InvalidParametersException, "Type of the file provided is not supported by this importer. File type: " + fileExtension);
		}

		FREE_IMAGE_FORMAT imageFormat = (FREE_IMAGE_FORMAT)findFormat->second;

		// Set error handler
		FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);

		// Buffer stream into memory (TODO: override IO functions instead?)
		MemoryDataStream memStream(fileData, true);

		FIMEMORY* fiMem = FreeImage_OpenMemory(memStream.getPtr(), static_cast<DWORD>(memStream.size()));

		FIBITMAP* fiBitmap = FreeImage_LoadFromMemory(
			(FREE_IMAGE_FORMAT)imageFormat, fiMem);
		if (!fiBitmap)
		{
			CM_EXCEPT(InternalErrorException, "Error decoding image");
		}

		RawImageData imageData;
		imageData.width = FreeImage_GetWidth(fiBitmap);
		imageData.height = FreeImage_GetHeight(fiBitmap);
		imageData.format = PF_UNKNOWN;

		// Must derive format first, this may perform conversions

		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
		FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(fiBitmap);
		unsigned bpp = FreeImage_GetBPP(fiBitmap);

		switch(imageType)
		{
		case FIT_UNKNOWN:
		case FIT_COMPLEX:
		case FIT_UINT32:
		case FIT_INT32:
		case FIT_DOUBLE:
		default:
			CM_EXCEPT(ItemIdentityException, "Unknown or unsupported image format");

			break;
		case FIT_BITMAP:
			// Standard image type
			// Perform any colour conversions for greyscale
			if (colourType == FIC_MINISWHITE || colourType == FIC_MINISBLACK)
			{
				FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fiBitmap);
				// free old bitmap and replace
				FreeImage_Unload(fiBitmap);
				fiBitmap = newBitmap;
				// get new formats
				bpp = FreeImage_GetBPP(fiBitmap);
				colourType = FreeImage_GetColorType(fiBitmap);
			}
			// Perform any colour conversions for RGB
			else if (bpp < 8 || colourType == FIC_PALETTE || colourType == FIC_CMYK)
			{
				FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(fiBitmap);
				// free old bitmap and replace
				FreeImage_Unload(fiBitmap);
				fiBitmap = newBitmap;
				// get new formats
				bpp = FreeImage_GetBPP(fiBitmap);
				colourType = FreeImage_GetColorType(fiBitmap);
			}

			// by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
			switch(bpp)
			{
			case 8:
				imageData.format = PF_L8;
				break;
			case 16:
				// Determine 555 or 565 from green mask
				// cannot be 16-bit greyscale since that's FIT_UINT16
				if(FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
				{
					imageData.format = PF_R5G6B5;
				}
				else
				{
					// FreeImage doesn't support 4444 format so must be 1555
					imageData.format = PF_A1R5G5B5;
				}
				break;
			case 24:
				// FreeImage differs per platform
				//     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
				//     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
				imageData.format = PF_BYTE_RGB;
#else
				imageData.format = PF_BYTE_BGR;
#endif
				break;
			case 32:
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
				imageData.format = PF_BYTE_RGBA;
#else
				imageData.format = PF_BYTE_BGRA;
#endif
				break;


			};
			break;
		case FIT_UINT16:
		case FIT_INT16:
			// 16-bit greyscale
			imageData.format = PF_L16;
			break;
		case FIT_FLOAT:
			// Single-component floating point data
			imageData.format = PF_FLOAT32_R;
			break;
		case FIT_RGB16:
			imageData.format = PF_SHORT_RGB;
			break;
		case FIT_RGBA16:
			imageData.format = PF_SHORT_RGBA;
			break;
		case FIT_RGBF:
			imageData.format = PF_FLOAT32_RGB;
			break;
		case FIT_RGBAF:
			imageData.format = PF_FLOAT32_RGBA;
			break;


		};

		unsigned char* srcData = FreeImage_GetBits(fiBitmap);
		unsigned srcPitch = FreeImage_GetPitch(fiBitmap);

		// Final data - invert image and trim pitch at the same time
		size_t dstPitch = imageData.width * PixelUtil::getNumElemBytes(imageData.format);
		size_t size = dstPitch * imageData.height;
		// Bind output buffer
		MemoryDataStreamPtr output(new MemoryDataStream(size));

		UINT8* pSrc;
		UINT8* pDst = output->getPtr();
		for (size_t y = 0; y < imageData.height; ++y)
		{
			pSrc = srcData + (imageData.height - y - 1) * srcPitch;
			memcpy(pDst, pSrc, dstPitch);
			pDst += dstPitch;
		}

		FreeImage_Unload(fiBitmap);
		FreeImage_CloseMemory(fiMem);

		imageData.data = output;

		return imageData;
	}
}