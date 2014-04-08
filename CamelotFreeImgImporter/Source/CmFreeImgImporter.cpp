#include "CmFreeImgImporter.h"
#include "CmResource.h"
#include "CmDebug.h"
#include "CmDataStream.h"
#include "CmPath.h"
#include "CmTextureData.h"
#include "CmTextureManager.h"
#include "CmTexture.h"
#include "CmFileSystem.h"
#include "CmApplication.h"
#include "CmCoreThread.h"
#include "CmCoreThreadAccessor.h"

#include "FreeImage.h"

namespace CamelotFramework
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
		WStringStream strExt;
		strExt << "Supported formats: ";
		bool first = true;
		for (int i = 0; i < FreeImage_GetFIFCount(); ++i)
		{

			// Skip DDS codec since FreeImage does not have the option 
			// to keep DXT data compressed, we'll use our own codec
			if ((FREE_IMAGE_FORMAT)i == FIF_DDS)
				continue;

			WString exts = toWString(String(FreeImage_GetFIFExtensionList((FREE_IMAGE_FORMAT)i)));
			if (!first)
			{
				strExt << ",";
			}
			first = false;
			strExt << exts;

			// Pull off individual formats (separated by comma by FI)
			Vector<WString>::type extsVector = StringUtil::split(exts, L",");
			for (auto v = extsVector.begin(); v != extsVector.end(); ++v)
			{
				auto findIter = std::find(mExtensions.begin(), mExtensions.end(), *v);

				if(findIter == mExtensions.end())
				{
					WString ext = *v;
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

	bool FreeImgImporter::isExtensionSupported(const WString& ext) const
	{
		WString lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FreeImgImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		WString ext = magicNumToExtension(magicNumPtr, numBytes);

		return isExtensionSupported(ext);
	}

	WString FreeImgImporter::magicNumToExtension(const UINT8* magic, UINT32 maxBytes) const
	{
		// Set error handler
		FreeImage_SetOutputMessage(FreeImageLoadErrorHandler);

		FIMEMORY* fiMem = 
			FreeImage_OpenMemory((BYTE*)magic, static_cast<DWORD>(maxBytes));

		FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(fiMem, (int)maxBytes);
		FreeImage_CloseMemory(fiMem);

		if (fif != FIF_UNKNOWN)
		{
			WString ext = toWString(String(FreeImage_GetFormatFromFIF(fif)));
			StringUtil::toLowerCase(ext);
			return ext;
		}
		else
		{
			return StringUtil::WBLANK;
		}
	}

	ResourcePtr FreeImgImporter::import(const WString& filePath, ConstImportOptionsPtr importOptions)
	{
		DataStreamPtr fileData = FileSystem::openFile(filePath, true);

		TextureDataPtr imgData = importRawImage(fileData);
		if(imgData == nullptr || imgData->getData() == nullptr)
			return nullptr;

		TexturePtr newTexture = Texture::_createPtr(TEX_TYPE_2D, 
			imgData->getWidth(), imgData->getHeight(), imgData->getNumMipmaps(), imgData->getFormat());

		newTexture->synchronize(); // TODO - Required due to a bug in allocateSubresourceBuffer

		for(UINT32 mip = 0; mip <= imgData->getNumMipmaps(); ++mip)
		{
			UINT32 subresourceIdx = newTexture->mapToSubresourceIdx(0, mip);
			PixelDataPtr src = newTexture->allocateSubresourceBuffer(subresourceIdx);

			imgData->getPixels(mip, *src);

			src->lock();
			gCoreThread().queueReturnCommand(boost::bind(&RenderSystem::writeSubresource, RenderSystem::instancePtr(), newTexture, subresourceIdx, src, false, _1));
		}

		fileData->close();

		WString fileName = Path::getFilename(filePath, false);
		newTexture->setName(toString(fileName));

		return newTexture;
	}

	TextureDataPtr FreeImgImporter::importRawImage(DataStreamPtr fileData)
	{
		if(fileData->size() > std::numeric_limits<UINT32>::max())
		{
			CM_EXCEPT(InternalErrorException, "File size larger than supported!");
		}

		UINT32 magicLen = std::min((UINT32)fileData->size(), 32u);
		UINT8 magicBuf[32];
		fileData->read(magicBuf, magicLen);
		// return to start
		fileData->seek(0);

		WString fileExtension = magicNumToExtension(magicBuf, magicLen);
		auto findFormat = mExtensionToFID.find(fileExtension);
		if(findFormat == mExtensionToFID.end())
		{
			CM_EXCEPT(InvalidParametersException, "Type of the file provided is not supported by this importer. File type: " + toString(fileExtension));
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

		UINT32 width = FreeImage_GetWidth(fiBitmap);
		UINT32 height = FreeImage_GetHeight(fiBitmap);
		PixelFormat format = PF_UNKNOWN;

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
				format = PF_R8;
				break;
			case 16:
				// Determine 555 or 565 from green mask
				// cannot be 16-bit greyscale since that's FIT_UINT16
				if(FreeImage_GetGreenMask(fiBitmap) == FI16_565_GREEN_MASK)
				{
					assert(false && "Format not supported by the engine. TODO.");
					return nullptr;
				}
				else
				{
					assert(false && "Format not supported by the engine. TODO.");
					return nullptr;
					// FreeImage doesn't support 4444 format so must be 1555
				}
				break;
			case 24:
				// FreeImage differs per platform
				//     PF_BYTE_BGR[A] for little endian (== PF_ARGB native)
				//     PF_BYTE_RGB[A] for big endian (== PF_RGBA native)
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
				format = PF_BYTE_RGB;
#else
				format = PF_BYTE_BGR;
#endif
				break;
			case 32:
#if FREEIMAGE_COLORORDER == FREEIMAGE_COLORORDER_RGB
				format = PF_BYTE_RGBA;
#else
				format = PF_BYTE_BGRA;
#endif
				break;


			};
			break;
		case FIT_UINT16:
		case FIT_INT16:
			// 16-bit greyscale
			assert(false && "No INT pixel formats supported currently. TODO.");
			return nullptr;
			break;
		case FIT_FLOAT:
			// Single-component floating point data
			format = PF_FLOAT32_R;
			break;
		case FIT_RGB16:
			format = PF_FLOAT16_RGB;
			break;
		case FIT_RGBA16:
			format = PF_FLOAT16_RGBA;
			break;
		case FIT_RGBF:
			format = PF_FLOAT32_RGB;
			break;
		case FIT_RGBAF:
			format = PF_FLOAT32_RGBA;
			break;
		};

		unsigned char* srcData = FreeImage_GetBits(fiBitmap);
		unsigned srcPitch = FreeImage_GetPitch(fiBitmap);

		// Final data - invert image and trim pitch at the same time
		UINT32 dstPitch = width * PixelUtil::getNumElemBytes(format);
		UINT32 size = dstPitch * height;

		// Bind output buffer
		TextureDataPtr texData = cm_shared_ptr<TextureData, ScratchAlloc>(width, height, size, format, 1, 0, 0);
		UINT8* output = texData->getData();

		UINT8* pSrc;
		UINT8* pDst = output;
		for (UINT32 y = 0; y < height; ++y)
		{
			pSrc = srcData + (height - y - 1) * srcPitch;
			memcpy(pDst, pSrc, dstPitch);
			pDst += dstPitch;
		}

		FreeImage_Unload(fiBitmap);
		FreeImage_CloseMemory(fiMem);

		return texData;
	}
}