#include "BsTextSprite.h"
#include "BsGUIMaterialManager.h"
#include "BsTextData.h"
#include "BsFont.h"
#include "BsVector2.h"

#include "BsProfilerCPU.h" // PROFILING ONLY

namespace BansheeEngine
{
	TextSprite::TextSprite()
	{

	}

	void TextSprite::update(const TEXT_SPRITE_DESC& desc, UINT64 groupId)
	{
		gProfilerCPU().beginSample("UpdateTextSprite");

		TextData textData(desc.text, desc.font, desc.fontSize, desc.width, desc.height, desc.wordWrap, desc.wordBreak);

		UINT32 numLines = textData.getNumLines();
		UINT32 numPages = textData.getNumPages();

		// Resize cached mesh array to needed size
		if(mCachedRenderElements.size() > numPages)
		{
			for(UINT32 i = numPages; i < (UINT32)mCachedRenderElements.size(); i++)
			{
				auto& renderElem = mCachedRenderElements[i];

				UINT32 vertexCount = renderElem.numQuads * 4;
				UINT32 indexCount = renderElem.numQuads * 6;

				if(renderElem.vertices != nullptr)
					bs_deleteN<ScratchAlloc>(renderElem.vertices, vertexCount);

				if(renderElem.uvs != nullptr)
					bs_deleteN<ScratchAlloc>(renderElem.uvs, vertexCount);

				if(renderElem.indexes != nullptr)
					bs_deleteN<ScratchAlloc>(renderElem.indexes, indexCount);

				if(renderElem.matInfo.material != nullptr)
				{
					GUIMaterialManager::instance().releaseMaterial(renderElem.matInfo);
				}
			}
		}

		if(mCachedRenderElements.size() != numPages)
			mCachedRenderElements.resize(numPages);

		// Actually generate a mesh
		UINT32 texPage = 0;
		for(auto& cachedElem : mCachedRenderElements)
		{
			UINT32 newNumQuads = textData.getNumQuadsForPage(texPage);
			if(newNumQuads != cachedElem.numQuads)
			{
				UINT32 oldVertexCount = cachedElem.numQuads * 4;
				UINT32 oldIndexCount = cachedElem.numQuads * 6;

				if(cachedElem.vertices != nullptr) bs_deleteN<ScratchAlloc>(cachedElem.vertices, oldVertexCount);
				if(cachedElem.uvs != nullptr) bs_deleteN<ScratchAlloc>(cachedElem.uvs, oldVertexCount);
				if(cachedElem.indexes != nullptr) bs_deleteN<ScratchAlloc>(cachedElem.indexes, oldIndexCount);

				cachedElem.vertices = bs_newN<Vector2, ScratchAlloc>(newNumQuads * 4);
				cachedElem.uvs = bs_newN<Vector2, ScratchAlloc>(newNumQuads * 4);
				cachedElem.indexes = bs_newN<UINT32, ScratchAlloc>(newNumQuads * 6);
				cachedElem.numQuads = newNumQuads;
			}

			const HTexture& tex = textData.getTextureForPage(texPage);

			bool getNewMaterial = false;
			if(cachedElem.matInfo.material == nullptr)
				getNewMaterial = true;
			else
			{
				const GUIMaterialInfo* matInfo = GUIMaterialManager::instance().findExistingTextMaterial(groupId, tex, desc.color);
				if(matInfo == nullptr)
				{
					getNewMaterial = true;
				}
				else
				{
					if(matInfo->material != cachedElem.matInfo.material)
					{
						GUIMaterialManager::instance().releaseMaterial(cachedElem.matInfo);
						getNewMaterial = true;
					}
				}
			}

			if(getNewMaterial)
				cachedElem.matInfo = GUIMaterialManager::instance().requestTextMaterial(groupId, tex, desc.color);

			texPage++;
		}

		// Calc alignment and anchor offsets and set final line positions
		for(UINT32 j = 0; j < numPages; j++)
		{
			SpriteRenderElement& renderElem = mCachedRenderElements[j];

			genTextQuads(j, textData, desc.width, desc.height, desc.horzAlign, desc.vertAlign, desc.anchor, 
				renderElem.vertices, renderElem.uvs, renderElem.indexes, renderElem.numQuads);
		}

		updateBounds();

		gProfilerCPU().endSample("UpdateTextSprite");
	}

	UINT32 TextSprite::genTextQuads(UINT32 page, const TextData& textData, UINT32 width, UINT32 height, 
		TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 bufferSizeQuads)
	{
		UINT32 numLines = textData.getNumLines();
		UINT32 newNumQuads = textData.getNumQuadsForPage(page);

		Vector<Vector2I> alignmentOffsets = getAlignmentOffsets(textData, width, height, horzAlign, vertAlign);
		Vector2I offset = getAnchorOffset(anchor, width, height);

		UINT32 quadOffset = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextData::TextLine& line = textData.getLine(i);
			UINT32 writtenQuads = line.fillBuffer(page, vertices, uv, indices, quadOffset, bufferSizeQuads);

			Vector2I position = offset + alignmentOffsets[i];
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


	UINT32 TextSprite::genTextQuads(const TextData& textData, UINT32 width, UINT32 height, 
		TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 bufferSizeQuads)
	{
		UINT32 numLines = textData.getNumLines();
		UINT32 numPages = textData.getNumPages();

		Vector<Vector2I> alignmentOffsets = getAlignmentOffsets(textData, width, height, horzAlign, vertAlign);
		Vector2I offset = getAnchorOffset(anchor, width, height);

		UINT32 quadOffset = 0;
		
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextData::TextLine& line = textData.getLine(i);
			for(UINT32 j = 0; j < numPages; j++)
			{
				UINT32 writtenQuads = line.fillBuffer(j, vertices, uv, indices, quadOffset, bufferSizeQuads);

				Vector2I position = offset + alignmentOffsets[i];

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

	Vector<Vector2I> TextSprite::getAlignmentOffsets(const TextData& textData, 
		UINT32 width, UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign)
	{
		UINT32 numLines = textData.getNumLines();
		UINT32 curHeight = 0;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextData::TextLine& line = textData.getLine(i);
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
		Vector<Vector2I> lineOffsets;
		for(UINT32 i = 0; i < numLines; i++)
		{
			const TextData::TextLine& line = textData.getLine(i);

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

			lineOffsets.push_back(Vector2I(horzOffset, vertOffset + curY));
			curY += line.getYOffset();
		}

		return lineOffsets;
	}
}