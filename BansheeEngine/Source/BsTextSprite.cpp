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

		UINT32 curHeight = 0;
		for(auto& line : lines)
			curHeight += line.getYOffset();

		// Calc vertical alignment offset
		UINT32 vertDiff = std::max(0U, desc.height - curHeight);
		UINT32 vertOffset = 0;
		switch(desc.vertAlign)
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

		// Calc horizontal alignment offset and set final line positions
		Int2 offset = getAnchorOffset(desc.anchor, desc.width, desc.height);
		UINT32 numPages = (UINT32)quadsPerPage.size();
		UINT32 curY = 0;
		Vector<UINT32>::type faceOffsets(mCachedRenderElements.size(), 0);
		for(size_t i = 0; i < lines.size(); i++)
		{
			UINT32 horzOffset = 0;
			switch(desc.horzAlign)
			{
			case THA_Left:
				horzOffset = 0;
				break;
			case THA_Right:
				horzOffset = std::max(0, (INT32)(desc.width - lines[i].getWidth()));
				break;
			case THA_Center:
				horzOffset = std::max(0, (INT32)(desc.width - lines[i].getWidth())) / 2;
				break;
			}

			Int2 position = offset + Int2(horzOffset, vertOffset + curY);
			curY += lines[i].getYOffset();

			for(size_t j = 0; j < numPages; j++)
			{
				SpriteRenderElement& renderElem = mCachedRenderElements[j];
				UINT32 offset = faceOffsets[j];

				UINT32 writtenQuads = lines[i].fillBuffer((UINT32)j, renderElem.vertices, renderElem.uvs, renderElem.indexes, offset, renderElem.numQuads);
				
				UINT32 numVertices = writtenQuads * 4;
				for(size_t i = 0; i < numVertices; i++)
				{
					renderElem.vertices[offset * 4 + i].x += (float)position.x;
					renderElem.vertices[offset * 4 + i].y += (float)position.y;
				}

				faceOffsets[j] += writtenQuads;
			}
		}

		if(desc.clipRect.width > 0 && desc.clipRect.height > 0)
		{
			for(auto& renderElem : mCachedRenderElements)
				clipToRect(renderElem.vertices, renderElem.uvs, renderElem.numQuads, desc.clipRect);
		}

		// Apply offset
		for(auto& renderElem : mCachedRenderElements)
		{
			UINT32 numVertices = renderElem.numQuads * 4;
			for(size_t i = 0; i < numVertices; i++)
			{
				renderElem.vertices[i].x += (float)desc.offset.x;
				renderElem.vertices[i].y += (float)desc.offset.y;
			}
		}

		// Store cached line data
		mLineDescs.clear();
		UINT32 curCharIdx = 0;
		UINT32 cachedLineY = 0;
		for(auto& line : lines)
		{
			SpriteLineDesc lineDesc;
			lineDesc.startChar = curCharIdx;
			lineDesc.endChar = curCharIdx + line.getNumChars();
			lineDesc.lineHeight = line.getYOffset();
			lineDesc.lineYStart = vertOffset + cachedLineY + desc.offset.y;

			mLineDescs.push_back(lineDesc);

			curCharIdx = lineDesc.endChar;
			cachedLineY += lineDesc.lineHeight;
		}

		updateBounds();
	}

	CM::Rect TextSprite::getCharRect(UINT32 charIdx) const
	{
		UINT32 idx = 0;
		for(auto& renderElem : mCachedRenderElements)
		{
			if(charIdx >= idx && charIdx < renderElem.numQuads)
			{
				UINT32 localIdx = (charIdx - idx) * 4;

				Rect charRect;
				charRect.x = Math::RoundToInt(renderElem.vertices[localIdx + 0].x);
				charRect.y = Math::RoundToInt(renderElem.vertices[localIdx + 0].y);
				charRect.width = Math::RoundToInt(renderElem.vertices[localIdx + 3].x - charRect.x);
				charRect.height = Math::RoundToInt(renderElem.vertices[localIdx + 3].y - charRect.y);

				return charRect;
			}

			idx += renderElem.numQuads;
		}

		CM_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}

	UINT32 TextSprite::getCharIdxAtPos(const Int2& pos) const
	{
		Vector2 vecPos((float)pos.x, (float)pos.y);

		float nearestDist = std::numeric_limits<float>::max();
		UINT32 nearestChar = 0;
		UINT32 idx = 0;
		for(auto& renderElem : mCachedRenderElements)
		{
			for(UINT32 i = 0; i < renderElem.numQuads; i++)
			{
				UINT32 curVert = idx * 4;

				Vector2 center = renderElem.vertices[curVert + 0] + renderElem.vertices[curVert + 1] + 
					renderElem.vertices[curVert + 2] + renderElem.vertices[curVert + 3];
				center /= 4;

				float dist = center.squaredDistance(vecPos);
				if(dist < nearestDist)
				{
					nearestChar = idx;
					nearestDist = dist;
				}

				idx++;
			}
		}

		return nearestChar;
	}

	CM::UINT32 TextSprite::getLineForChar(CM::UINT32 charIdx) const
	{
		UINT32 idx = 0;
		for(auto& line : mLineDescs)
		{
			if(charIdx >= line.startChar && charIdx < line.endChar)
			{
				return idx;
			}

			idx++;
		}
		
		CM_EXCEPT(InternalErrorException, "Invalid character index: " + toString(charIdx));
	}
}