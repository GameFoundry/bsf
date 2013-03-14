#include "CmFontImporter.h"
#include "CmFontImportOptions.h"
#include "CmPixelData.h"
#include "CmTexture.h"
#include "CmResources.h"

#include <ft2build.h>
#include <freetype/freetype.h>
#include FT_FREETYPE_H

namespace CamelotEngine
{
	FontImporter::FontImporter()
		:SpecificImporter() 
	{
		mExtensions.push_back("ttf");
		mExtensions.push_back("otf");
	}

	FontImporter::~FontImporter() 
	{

	}

	bool FontImporter::isExtensionSupported(const String& ext) const
	{
		String lowerCaseExt = ext;
		StringUtil::toLowerCase(lowerCaseExt);

		return find(mExtensions.begin(), mExtensions.end(), lowerCaseExt) != mExtensions.end();
	}

	bool FontImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		// TODO
		return false;
	}

	BaseResourceHandle FontImporter::import(const String& filePath, ConstImportOptionsPtr importOptions)
	{
		const FontImportOptions* gpuProgImportOptions = static_cast<const FontImportOptions*>(importOptions.get());

		FT_Library library;

		FT_Error error = FT_Init_FreeType(&library);
		if (error)
			CM_EXCEPT(InternalErrorException, "Error occurred during FreeType library initialization.");

		FT_Face face;
		error = FT_New_Face(library, filePath.c_str(), 0, &face);

		if (error == FT_Err_Unknown_File_Format)
		{
			CM_EXCEPT(InternalErrorException, "Failed to load font file: " + filePath + ". Unsupported file format.");
		}
		else if (error)
		{
			CM_EXCEPT(InternalErrorException, "Failed to load font file: " + filePath + ". Unknown error.");
		}

		vector<std::pair<UINT32, UINT32>>::type charIndexRanges = gpuProgImportOptions->getCharIndexRanges();
		vector<UINT32>::type fontSizes = gpuProgImportOptions->getFontSizes();
		UINT32 dpi = gpuProgImportOptions->getDPI();
		for(size_t i = 0; i < fontSizes.size(); i++)
		{
			FT_F26Dot6 ftSize = (FT_F26Dot6)(fontSizes[i] * (1 << 6));
			if(FT_Set_Char_Size( face, ftSize, 0, dpi, dpi))
				CM_EXCEPT(InternalErrorException, "Could not set character size." );

			
		}

		/************************************************************************/
		/* 									DEBUG				         		*/
		/************************************************************************/
		
		if(FT_Set_Char_Size(face, 12 * (1 << 6), 0, 96, 96))
			CM_EXCEPT(InternalErrorException, "Could not set character size." );

		INT32 maxWidth = 0;
		INT32 maxHeight = 0;
		for(UINT32 i = 33; i < 166; i++)
		{
			FT_Load_Char(face, i, FT_LOAD_RENDER);

			FT_GlyphSlot slot = face->glyph;

			if(slot->bitmap.width > maxWidth)
				maxWidth = slot->bitmap.width;

			if(slot->bitmap.rows > maxHeight)
				maxHeight = slot->bitmap.rows;
		}

		UINT32 texWidth = 1024;
		UINT32 texHeight = 1024;

		UINT8* pixelBuffer = new UINT8[texWidth * texHeight];
		PixelData pixelData(texWidth, texHeight, 1, PF_R8G8, pixelBuffer, true);

		UINT32 curX = 0;
		UINT32 curY = 0;
		for(UINT32 i = 33; i < 166; i++)
		{
			if((curX + maxWidth) >= texWidth)
			{
				curX = 0;
				curY += maxHeight;

				if((curY + maxHeight) >= texHeight)
					CM_EXCEPT(InternalErrorException, "Texture to small to fit all glyphs.");
			}

			FT_Load_Char(face, i, FT_LOAD_RENDER);
			FT_GlyphSlot slot = face->glyph;

			if(slot->bitmap.buffer == nullptr)
			{
				CM_EXCEPT(InternalErrorException, "Failed to render glyph bitmap");
			}

			UINT8* sourceBuffer = slot->bitmap.buffer;
			UINT8* dstBuffer = pixelBuffer + (curY * texWidth * 2) + curX * 2;
			for(INT32 bitmapRow = 0; bitmapRow < slot->bitmap.rows; bitmapRow++)
			{
				for(INT32 bitmapColumn = 0; bitmapColumn < slot->bitmap.width; bitmapColumn++)
				{
					dstBuffer[bitmapColumn * 2 + 0] = sourceBuffer[bitmapColumn];
					dstBuffer[bitmapColumn * 2 + 1] = sourceBuffer[bitmapColumn];
				}

				dstBuffer += texWidth * 2;
				sourceBuffer += slot->bitmap.pitch;
			}

			curX += maxWidth;
		}

		TextureHandle newTex = Texture::create(TEX_TYPE_2D, texWidth, texHeight, 0, PF_R8G8);
		newTex->setRawPixels(pixelData);

		Resources::instance().create(newTex, "C:\\FontTex.tex", true);

		/************************************************************************/
		/* 								END DEBUG	                     		*/
		/************************************************************************/

		
		FT_Done_FreeType(library);

		return BaseResourceHandle();
	}
}