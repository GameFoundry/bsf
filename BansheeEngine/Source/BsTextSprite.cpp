#include "BsTextSprite.h"
#include "BsGUIMaterialManager.h"
#include "CmTextUtility.h"
#include "CmFont.h"
#include "CmVector2.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	TextSprite::TextSprite()
	{

	}

	void TextSprite::update(const TEXT_SPRITE_DESC& desc)
	{
		mLineDescs.clear();

		std::shared_ptr<TextUtility::TextData> textData = TextUtility::getTextData(desc.text, desc.font, desc.fontSize, desc.width, desc.height, desc.wordWrap);

		if(textData == nullptr)
			return;

		const CM::Vector<TextUtility::TextLine>::type& lines = textData->getLines();
		const CM::Vector<UINT32>::type& quadsPerPage = textData->getNumQuadsPerPage();

		// Resize cached mesh array to needed size
		if(mCachedRenderElements.size() > quadsPerPage.size())
		{
			for(UINT32 i = (UINT32)quadsPerPage.size(); i < (UINT32)mCachedRenderElements.size(); i++)
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

		if(mCachedRenderElements.size() != quadsPerPage.size())
			mCachedRenderElements.resize(quadsPerPage.size());

		// Actually generate a mesh
		const CM::Vector<HTexture>::type& texturePages = textData->getTexturePages();
		UINT32 texPage = 0;
		for(auto& cachedElem : mCachedRenderElements)
		{
			UINT32 newNumQuads = quadsPerPage[texPage];
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

			HMaterial newMaterial = GUIMaterialManager::instance().requestTextMaterial(texturePages[texPage]);
			if(cachedElem.material != nullptr)
				GUIMaterialManager::instance().releaseMaterial(newMaterial);

			cachedElem.material = newMaterial;

			texPage++;
		}

		// Calc alignment and anchor offsets and set final line positions
		UINT32 numPages = (UINT32)quadsPerPage.size();
		
		for(UINT32 j = 0; j < numPages; j++)
		{
			SpriteRenderElement& renderElem = mCachedRenderElements[j];

			genTextQuads(j, *textData, desc.width, desc.height, desc.horzAlign, desc.vertAlign, desc.anchor, 
				renderElem.vertices, renderElem.uvs, renderElem.indexes, renderElem.numQuads);
		}

		// Store cached line data
		UINT32 curCharIdx = 0;
		UINT32 cachedLineY = 0;
		UINT32 curLineIdx = 0;
		Vector<Int2>::type alignmentOffsets = getAlignmentOffsets(lines, desc.width, desc.height, desc.horzAlign, desc.vertAlign);
		for(auto& line : lines)
		{
			// Line has a newline char only if it wasn't created by word wrap and it isn't the last line
			bool hasNewline = line.hasNewlineChar() && (curLineIdx != ((UINT32)lines.size() - 1));

			UINT32 startChar = curCharIdx;
			UINT32 endChar = curCharIdx + line.getNumChars() + (hasNewline ? 1 : 0);
			UINT32 lineHeight = line.getYOffset();
			INT32 lineYStart = alignmentOffsets[curLineIdx].y + 0; // TODO - Ignoring offset

			SpriteLineDesc lineDesc(startChar, endChar, lineHeight, lineYStart, hasNewline);
			mLineDescs.push_back(lineDesc);

			curCharIdx = lineDesc.getEndChar();
			cachedLineY += lineDesc.getLineHeight();
			curLineIdx++;
		}

		updateBounds();
	}

	UINT32 TextSprite::genTextQuads(UINT32 page, const TextUtility::TextData& textData, UINT32 width, UINT32 height, 
		TextHorzAlign horzAlign, TextVertAlign vertAlign, SpriteAnchor anchor, Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 bufferSizeQuads)
	{
		const CM::Vector<TextUtility::TextLine>::type& lines = textData.getLines();
		const CM::Vector<UINT32>::type& quadsPerPage = textData.getNumQuadsPerPage();

		UINT32 newNumQuads = quadsPerPage[page];

		Vector<Int2>::type alignmentOffsets = getAlignmentOffsets(lines, width, height, horzAlign, vertAlign);
		Int2 offset = getAnchorOffset(anchor, width, height);

		UINT32 quadOffset = 0;
		for(size_t i = 0; i < lines.size(); i++)
		{
			UINT32 writtenQuads = lines[i].fillBuffer(page, vertices, uv, indices, quadOffset, bufferSizeQuads);

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
		const CM::Vector<TextUtility::TextLine>::type& lines = textData.getLines();
		const CM::Vector<UINT32>::type& quadsPerPage = textData.getNumQuadsPerPage();

		Vector<Int2>::type alignmentOffsets = getAlignmentOffsets(lines, width, height, horzAlign, vertAlign);
		Int2 offset = getAnchorOffset(anchor, width, height);

		UINT32 quadOffset = 0;
		UINT32 numPages = (UINT32)quadsPerPage.size();
		for(size_t i = 0; i < lines.size(); i++)
		{
			for(UINT32 j = 0; j < numPages; j++)
			{
				UINT32 writtenQuads = lines[i].fillBuffer(j, vertices, uv, indices, quadOffset, bufferSizeQuads);

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

	CM::Rect TextSprite::getCharRect(UINT32 charIdx) const
	{
		UINT32 lineIdx = getLineForChar(charIdx);

		// If char is newline we don't have any geometry to return
		const SpriteLineDesc& lineDesc = getLineDesc(lineIdx);
		if(lineDesc.isNewline(charIdx))
			return Rect();

		UINT32 numNewlineChars = 0;
		for(UINT32 i = 0; i < lineIdx; i++)
			numNewlineChars += (getLineDesc(i).hasNewlineChar() ? 1 : 0);

		UINT32 quadIdx = charIdx - numNewlineChars;

		UINT32 curQuadIdx = 0;
		for(auto& renderElem : mCachedRenderElements)
		{
			if(quadIdx >= curQuadIdx && quadIdx < renderElem.numQuads)
			{
				UINT32 localIdx = (quadIdx - curQuadIdx) * 4;

				Rect charRect;
				charRect.x = Math::RoundToInt(renderElem.vertices[localIdx + 0].x);
				charRect.y = Math::RoundToInt(renderElem.vertices[localIdx + 0].y);
				charRect.width = Math::RoundToInt(renderElem.vertices[localIdx + 3].x - charRect.x);
				charRect.height = Math::RoundToInt(renderElem.vertices[localIdx + 3].y - charRect.y);

				return charRect;
			}

			curQuadIdx += renderElem.numQuads;
		}

		CM_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}

	INT32 TextSprite::getCharIdxAtPos(const Int2& pos) const
	{
		Vector2 vecPos((float)pos.x, (float)pos.y);

		UINT32 lineStartChar = 0;
		UINT32 lineEndChar = 0;
		UINT32 numNewlineChars = 0;
		UINT32 lineIdx = 0;
		for(auto& line : mLineDescs)
		{
			if(pos.y >= line.getLineYStart() && pos.y < (line.getLineYStart() + (INT32)line.getLineHeight()))
			{
				lineStartChar = line.getStartChar();
				lineEndChar = line.getEndChar(false);
				break;
			}

			// Newline chars count in the startChar/endChar variables, but don't actually exist in the buffers
			// so we need to filter them out
			numNewlineChars += (line.hasNewlineChar() ? 1 : 0); 

			lineIdx++;
		}

		UINT32 lineStartQuad = lineStartChar - numNewlineChars;
		UINT32 lineEndQuad = lineEndChar - numNewlineChars;

		float nearestDist = std::numeric_limits<float>::max();
		UINT32 nearestChar = 0;
		UINT32 quadIdx = 0;
		bool foundChar = false;
		for(auto& renderElem : mCachedRenderElements)
		{
			for(UINT32 i = 0; i < renderElem.numQuads; i++)
			{
				if(quadIdx < lineStartQuad)
				{
					quadIdx++;
					continue;
				}

				if(quadIdx >= lineEndQuad)
					break;

				UINT32 curVert = quadIdx * 4;

				float centerX = renderElem.vertices[curVert + 0].x + renderElem.vertices[curVert + 1].x;
				centerX *= 0.5f;

				float dist = Math::Abs(centerX - vecPos.x);
				if(dist < nearestDist)
				{
					nearestChar = quadIdx + numNewlineChars;
					nearestDist = dist;
					foundChar = true;
				}

				quadIdx++;
			}
		}

		if(!foundChar)
			return -1;

		return nearestChar;
	}

	CM::UINT32 TextSprite::getLineForChar(CM::UINT32 charIdx, bool newlineCountsOnNextLine) const
	{
		UINT32 idx = 0;
		for(auto& line : mLineDescs)
		{
			if(charIdx >= line.getStartChar() && charIdx < line.getEndChar())
			{
				if(line.isNewline(charIdx) && newlineCountsOnNextLine)
					return idx + 1; // Incrementing is safe because next line must exist, since we just found a newline char

				return idx;
			}

			idx++;
		}
		
		CM_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}

	Vector<Int2>::type TextSprite::getAlignmentOffsets(const Vector<TextUtility::TextLine>::type& lines, 
		UINT32 width, UINT32 height, TextHorzAlign horzAlign, TextVertAlign vertAlign)
	{
		UINT32 curHeight = 0;
		for(auto& line : lines)
			curHeight += line.getYOffset();

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
		for(size_t i = 0; i < lines.size(); i++)
		{
			UINT32 horzOffset = 0;
			switch(horzAlign)
			{
			case THA_Left:
				horzOffset = 0;
				break;
			case THA_Right:
				horzOffset = std::max(0, (INT32)(width - lines[i].getWidth()));
				break;
			case THA_Center:
				horzOffset = std::max(0, (INT32)(width - lines[i].getWidth())) / 2;
				break;
			}

			lineOffsets.push_back(Int2(horzOffset, vertOffset + curY));
			curY += lines[i].getYOffset();
		}

		return lineOffsets;
	}

	SpriteLineDesc::SpriteLineDesc(CM::UINT32 startChar, CM::UINT32 endChar, CM::UINT32 lineHeight, CM::INT32 lineYStart, bool includesNewline)
		:mStartChar(startChar), mEndChar(endChar), mLineHeight(lineHeight), mLineYStart(lineYStart), mIncludesNewline(includesNewline)
	{

	}

	UINT32 SpriteLineDesc::getEndChar(bool includeNewline) const
	{
		if(mIncludesNewline)
		{
			if(includeNewline)
				return mEndChar;
			else
			{
				if(mEndChar > 0)
					return mEndChar - 1;
				else
					return mStartChar;
			}
		}
		else
			return mEndChar;
	}

	bool SpriteLineDesc::isNewline(UINT32 charIdx) const
	{
		if(mIncludesNewline)
		{
			return (mEndChar - 1) == charIdx;
		}
		else
			return false;
	}
}