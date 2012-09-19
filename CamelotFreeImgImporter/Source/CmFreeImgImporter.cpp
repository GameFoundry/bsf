#include "CmFreeImgImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmPixelUtil.h"

#include "FreeImage.h"

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
					mExtensions.push_back(*v);
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

	const std::vector<String>& FreeImgImporter::extensions() const
	{
		return mExtensions;
	}

	ResourcePtr FreeImgImporter::import(const String& filePath)
	{
		// TODO - Magic number to extension conversion? Its probably needed so I can detect file types on Linux
		
//		// Set error handler
//		FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);
//
//		// Buffer stream into memory (TODO: override IO functions instead?)
//		MemoryDataStream memStream(input, true);
//
//		FIMEMORY* fiMem = 
//			FreeImage_OpenMemory(memStream.getPtr(), static_cast<DWORD>(memStream.size()));
//
//		FIBITMAP* fiBitmap = FreeImage_LoadFromMemory(
//			(FREE_IMAGE_FORMAT)mFreeImageType, fiMem);
//		if (!fiBitmap)
//		{
//			CM_EXCEPT(InternalErrorException, "Error decoding image");
//		}
//
//
//		ImageData* imgData = OGRE_NEW ImageData();
//		MemoryDataStreamPtr output;
//
//		imgData->depth = 1; // only 2D formats handled by this codec
//		imgData->width = FreeImage_GetWidth(fiBitmap);
//		imgData->height = FreeImage_GetHeight(fiBitmap);
//		imgData->num_mipmaps = 0; // no mipmaps in non-DDS 
//		imgData->flags = 0;
//
//		// Must derive format first, this may perform conversions
//
//		FREE_IMAGE_TYPE imageType = FreeImage_GetImageType(fiBitmap);
//		FREE_IMAGE_COLOR_TYPE colourType = FreeImage_GetColorType(fiBitmap);
//		unsigned bpp = FreeImage_GetBPP(fiBitmap);
//
//		switch(imageType)
//		{
//		case FIT_UNKNOWN:
//		case FIT_COMPLEX:
//		case FIT_UINT32:
//		case FIT_INT32:
//		case FIT_DOUBLE:
//		default:
//			CM_EXCEPT(ItemIdentityException, "Unknown or unsupported image format");
//
//			break;
//		case FIT_BITMAP:
//			// Standard image type
//			// Perform any colour conversions for greyscale
//			if (colourType == FIC_MINISWHITE || colourType == FIC_MINISBLACK)
//			{
//				FIBITMAP* newBitmap = FreeImage_ConvertToGreyscale(fiBitmap);
//				// free old bitmap and replace
//				FreeImage_Unload(fiBitmap);
//				fiBitmap = newBitmap;
//				// get new formats
//				bpp = FreeImage_GetBPP(fiBitmap);
//				colourType = FreeImage_GetColorType(fiBitmap);
//			}
//			// Perform any colour conversions for RGB
//			else if (bpp < 8 || colourType == FIC_PALETTE || colourType == FIC_CMYK)
//			{
//				FIBITMAP* newBitmap = FreeImage_ConvertTo24Bits(fiBitmap);
//				// free old bitmap and replace
//				FreeImage_Unload(fiBitmap);
//				fiBitmap = newBitmap;
//				// get new formats
//				bpp = FreeImage_GetBPP(fiBitmap);
//				colourType = FreeImage_GetColorType(fiBitmap);
//			}
//
//			// by this stage, 8-bit is greyscale, 16/24/32 bit are RGB[A]
//			switch(bpp)
//			{
//			case 8:
//				imgData->format = PF_L8;
//				break;
//			case 16:
//				// Determine 555 or 565 from green mask
//				// cannot be 16-bit greyscale since that's FIT_UINT16
//				if(FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
//				{
//					imgData->format = PF_R5G6B5;
//				}
//				else
//				{
//					// FreeImage doesn't support 4444 format so must be 1555
//					imgData->format = PF_A1R5G5B5;
//				}
//				break;
//			case 24:
//				// FreeImage differs per platform
//				//     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
//				//     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
//				imgData->format = PF_BYTE_RGB;
//#else
//				imgData->format = PF_BYTE_BGR;
//#endif
//				break;
//			case 32:
//#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
//				imgData->format = PF_BYTE_RGBA;
//#else
//				imgData->format = PF_BYTE_BGRA;
//#endif
//				break;
//
//
//			};
//			break;
//		case FIT_UINT16:
//		case FIT_INT16:
//			// 16-bit greyscale
//			imgData->format = PF_L16;
//			break;
//		case FIT_FLOAT:
//			// Single-component floating point data
//			imgData->format = PF_FLOAT32_R;
//			break;
//		case FIT_RGB16:
//			imgData->format = PF_SHORT_RGB;
//			break;
//		case FIT_RGBA16:
//			imgData->format = PF_SHORT_RGBA;
//			break;
//		case FIT_RGBF:
//			imgData->format = PF_FLOAT32_RGB;
//			break;
//		case FIT_RGBAF:
//			imgData->format = PF_FLOAT32_RGBA;
//			break;
//
//
//		};
//
//		unsigned char* srcData = FreeImage_GetBits(fiBitmap);
//		unsigned srcPitch = FreeImage_GetPitch(fiBitmap);
//
//		// Final data - invert image and trim pitch at the same time
//		size_t dstPitch = imgData->width * PixelUtil::getNumElemBytes(imgData->format);
//		imgData->size = dstPitch * imgData->height;
//		// Bind output buffer
//		output.bind(OGRE_NEW MemoryDataStream(imgData->size));
//
//		uchar* pSrc;
//		uchar* pDst = output->getPtr();
//		for (size_t y = 0; y < imgData->height; ++y)
//		{
//			pSrc = srcData + (imgData->height - y - 1) * srcPitch;
//			memcpy(pDst, pSrc, dstPitch);
//			pDst += dstPitch;
//		}
//
//
//		FreeImage_Unload(fiBitmap);
//		FreeImage_CloseMemory(fiMem);
//
//		DecodeResult ret;
//		ret.first = output;
//		ret.second = CodecDataPtr(imgData);
//		return ret;


		return nullptr;
	}
}