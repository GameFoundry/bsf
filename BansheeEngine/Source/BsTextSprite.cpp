#include "BsTextSprite.h"
#include "BsGUIMaterialManager.h"
#include "CmTextUtility.h"
#include "CmFont.h"
#include "CmVector2.h"

#include "CmProfiler.h" // PROFILING ONLY

using namespace CamelotFramework;

namespace BansheeEngine
{
	TextSprite::TextSprite()
	{

	}

	void TextSprite::update(const TEXT_SPRITE_DESC& desc)
	{
		gProfiler().beginSample("textUpdateA");
		std::shared_ptr<TextUtility::TextData> textData = TextUtility::getTextData(desc.text, desc.font, desc.fontSize, desc.width, desc.height, desc.wordWrap);
		gProfiler().endSample("textUpdateA");

		if(textData == nullptr)
			return;

		gProfiler().beginSample("textUpdateB");

		UINT32 numLines = textData->getNumLines();
		UINT32 numPages = textData->getNumPages();

		// Resize cached mesh array to needed size
		if(mCachedRenderElements.size() > numPages)
		{
			for(UINT32 i = numPages; i < (UINT32)mCachedRenderElements.size(); i++)
			{
				auto& renderElem = mCachedRenderElements[i];

				UINT32 vertexCount = renderElem.numQuads * 4;
				UINT32 indexCount = renderElem.numQuads * 6;

				if(renderElem.vertices != nullptr)
					cm_deleteN<ScratchAlloc>(renderElem.vertices, vertexCount);

				if(renderElem.uvs != nullptr)
					cm_deleteN<ScratchAlloc>(renderElem.uvs, vertexCount);

				if(renderElem.indexes != nullptr)
					cm_deleteN<ScratchAlloc>(renderElem.indexes, indexCount);

				if(renderElem.material != nullptr)
				{
					GUIMaterialManager::instance().releaseMaterial(renderElem.material);
				}
			}
		}

		if(mCachedRenderElements.size() != numPages)
			mCachedRenderElements.resize(numPages);

		gProfiler().endSample("textUpdateB");
		gProfiler().beginSample("textUpdateC");

		// Actually generate a mesh
		UINT32 texPage = 0;
		for(auto& cachedElem : mCachedRenderElements)
		{
			UINT32 newNumQuads = textData->getNumQuadsForPage(texPage);
			if(newNumQuads != cachedElem.numQuads)
			{
				UINT32 oldVertexCount = cachedElem.numQuads * 4;
				UINT32 oldIndexCount = cachedElem.numQuads * 6;

				if(cachedElem.vertices != nullptr) cm_deleteN<ScratchAlloc>(cachedElem.vertices, oldVertexCount);
				if(cachedElem.uvs != nullptr) cm_deleteN<ScratchAlloc>(cachedElem.uvs, oldVertexCount);
				if(cachedElem.indexes != nullptr) cm_deleteN<ScratchAlloc>(cachedElem.indexes, oldIndexCount);

				cachedElem.vertices = cm_newN<Vector2, ScratchAlloc>(newNumQuads * 4);
				cachedElem.uvs = cm_newN<Vector2, ScratchAlloc>(newNumQuads * 4);
				cachedElem.indexes = cm_newN<UINT32, ScratchAlloc>(newNumQuads * 6);
				cachedElem.numQuads = newNumQuads;
			}

			HMaterial newMaterial = GUIMaterialManager::instance().requestTextMaterial(textData->getTextureForPage(texPage));
			if(cachedElem.material != nullptr)
				GUIMaterialManager::instance().releaseMaterial(cachedElem.material);

			cachedElem.material = newMaterial;

			texPage++;
		}

		gProfiler().instance().endSample("textUpdateC");
		gProfiler().instance().beginSample("textUpdateD");

		// Calc alignment and anchor offsets and set final line positions
		for(UINT32 j = 0; j < numPages; j++)
		{
			SpriteRenderElement& renderElem = mCachedRenderElements[j];

			genTextQuads(j, *textData, desc.width, desc.height, desc.horzAlign, desc.vertAlign, desc.anchor, 
				renderElem.vertices, renderElem.uvs, renderElem.indexes, renderElem.numQuads);
		}

		gProfiler().instance().endSample("textUpdateD");
		gProfiler().instance().beginSample("textUpdateE");

		updateBounds();

		gProfiler().instance().endSample("textUpdateE");
	}

	UINT32 TextSprite::genTextQuads(UINT32 page, const TextUtility::TextData& textData, UINT32 width, UINT32 height, 
		TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 bufferSizeQuads)
	{
		UINT32 numLines = textData.getNumLines();
		UINT32 newNumQuads = textData.getNumQuadsForPage(page);

		Vector<Int2>::type alignmentOffsets = getAlignmentOffsets(textData, width, height, horzAlign, vertAlign);
		Int2 offset = getAnchorOffset(anchor, width, height);

		UINT32 quadOffset = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextUtility::TextLine& line = textData.getLine(i);
			UINT32 writtenQuads = line.fillBuffer(page, vertices, uv, indices, quadOffset, bufferSizeQuads);

			Int2 position = offset + alignmentOffsets[i];
			UINT32 numVertices = writtenQuads * 4;
			for(UINT32 i = 0; i < numVertices; i++)
			{
				vertices[quadOffset * 4 + i].x += (float)position.x;
				vertices[quadOffset * 4 + i].y += (float)position.y;
			}

			quadOffset += writtenQuads;
		}

		return newNumQuads;
	}


	UINT32 TextSprite::genTextQuads(const TextUtility::TextData& textData, UINT32 width, UINT32 height, 
		TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 bufferSizeQuads)
	{
		UINT32 numLines = textData.getNumLines();
		UINT32 numPages = textData.getNumPages();

		Vector<Int2>::type alignmentOffsets = getAlignmentOffsets(textData, width, height, horzAlign, vertAlign);
		Int2 offset = getAnchorOffset(anchor, width, height);

		UINT32 quadOffset = 0;
		
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextUtility::TextLine& line = textData.getLine(i);
			for(UINT32 j = 0; j < numPages; j++)
			{
				UINT32 writtenQuads = line.fillBuffer(j, vertices, uv, indices, quadOffset, bufferSizeQuads);

				Int2 position = offset + alignmentOffsets[i];

				UINT32 numVertices = writtenQuads * 4;
				for(UINT32 i = 0; i < numVertices; i++)
				{
					vertices[quadOffset * 4 + i].x += (float)position.x;
					vertices[quadOffset * 4 + i].y += (float)position.y;
				}

				quadOffset += writtenQuads;
			}
		}

		return quadOffset;
	}

	Vector<Int2>::type TextSprite::getAlignmentOffsets(const TextUtility::TextData& textData, 
		UINT32 width, UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign)
	{
		UINT32 numLines = textData.getNumLines();
		UINT32 curHeight = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextUtility::TextLine& line = textData.getLine(i);
			curHeight += line.getYOffset();
		}

		// Calc vertical alignment offset
		UINT32 vertDiff = std::max(0U, height - curHeight);
		UINT32 vertOffset = 0;
		switch(vertAlign)
		{
		case TVA_Top:
			vertOffset = 0;
			break;
		case TVA_Bottom:
			vertOffset = std::max(0, (INT32)vertDiff);
			break;
		case TVA_Center:
			vertOffset = std::max(0, (INT32)vertDiff) / 2;
			break;
		}

		// Calc horizontal alignment offset
		UINT32 curY = 0;
		Vector<Int2>::type lineOffsets;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextUtility::TextLine& line = textData.getLine(i);

			UINT32 horzOffset = 0;
			switch(horzAlign)
			{
			case THA_Left:
				horzOffset = 0;
				break;
			case THA_Right:
				horzOffset = std::max(0, (INT32)(width - line.getWidth()));
				break;
			case THA_Center:
				horzOffset = std::max(0, (INT32)(width - line.getWidth())) / 2;
				break;
			}

			lineOffsets.push_back(Int2(horzOffset, vertOffset + curY));
			curY += line.getYOffset();
		}

		return lineOffsets;
	}
}