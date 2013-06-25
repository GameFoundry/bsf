#include "BsTextSprite.h"
#include "BsGUIMaterialManager.h"
#include "CmVector2.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	Sprite::Sprite()
	{

	}

	Sprite::~Sprite()
	{
		clearMesh();
	}

	UINT32 Sprite::getNumRenderElements() const
	{
		return (UINT32)mCachedRenderElements.size();
	}

	const HMaterial& Sprite::getMaterial(UINT32 renderElementIdx) const
	{
		return mCachedRenderElements.at(renderElementIdx).material;
	}

	UINT32 Sprite::getNumQuads(UINT32 renderElementIdx) const
	{
		return mCachedRenderElements.at(renderElementIdx).numQuads;
	}

	UINT32 Sprite::fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const
	{
		auto renderElem = mCachedRenderElements.at(renderElementIdx);

		UINT32 startVert = startingQuad * 4;
		UINT32 startIndex = startingQuad * 6;

		UINT32 maxVertIdx = maxNumQuads * 4;
		UINT32 maxIndexIdx = maxNumQuads * 6;

		UINT32 mNumVertices = renderElem.numQuads * 4;
		UINT32 mNumIndices = renderElem.numQuads * 6;

		assert((startVert + mNumVertices) <= maxVertIdx);
		assert((startIndex + mNumIndices) <= maxIndexIdx);

		UINT8* vertDst = vertices + startVert * vertexStride;
		UINT8* uvDst = uv + startVert * vertexStride;
		for(UINT32 i = 0; i < mNumVertices; i++)
		{
			memcpy(vertDst, &renderElem.vertices[i], sizeof(Vector2));
			memcpy(uvDst, &renderElem.uvs[i], sizeof(Vector2));

			vertDst += vertexStride;
			uvDst += vertexStride;
		}

		memcpy(&indices[startIndex], renderElem.indexes, mNumIndices * sizeof(UINT32));

		return renderElem.numQuads;
	}

	Int2 Sprite::getAnchorOffset(SpriteAnchor anchor, UINT32 width, UINT32 height)
	{
		switch(anchor)
		{
		case SA_TopLeft:
			return -Int2(0, 0);
		case SA_TopCenter:
			return -Int2(width / 2, 0);
		case SA_TopRight:
			return -Int2(width, 0);
		case SA_MiddleLeft:
			return -Int2(0, height / 2);
		case SA_MiddleCenter:
			return -Int2(width / 2, height / 2);
		case SA_MiddleRight:
			return -Int2(width, height / 2);
		case SA_BottomLeft:
			return -Int2(0, height);
		case SA_BottomCenter:
			return -Int2(width / 2, height);
		case SA_BottomRight:
			return -Int2(width, height);
		}

		return Int2();
	}

	void Sprite::updateBounds() const
	{
		Vector2 min;
		Vector2 max;

		// Find starting point
		for(auto& renderElem : mCachedRenderElements)
		{
			if(renderElem.vertices != nullptr && renderElem.numQuads > 0)
			{
				min = renderElem.vertices[0];
				max = renderElem.vertices[0];
				break;
			}
		}

		// Calculate bounds
		for(auto& renderElem : mCachedRenderElements)
		{
			if(renderElem.vertices != nullptr && renderElem.numQuads > 0)
			{
				UINT32 vertexCount = renderElem.numQuads * 4;

				for(UINT32 i = 0; i < vertexCount; i++)
				{
					min = Vector2::min(min, renderElem.vertices[i]);
					max = Vector2::max(max, renderElem.vertices[i]);
				}
			}
		}

		mBounds = Rect((int)min.x, (int)min.y, (int)(max.x - min.x), (int)(max.y - min.y));
	}

	void Sprite::clearMesh() const
	{
		for(auto& renderElem : mCachedRenderElements)
		{
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

		mCachedRenderElements.clear();
		updateBounds();
	}

	// This will only properly clip an array of rectangular quads
	// Vertices in the quad must be in a specific order: top left, top right, bottom left, bottom right
	// (0, 0) represents top left of the screen
	void Sprite::clipToRect(Vector2* vertices, Vector2* uv, UINT32 numQuads, const Rect& clipRect)
	{
		float left = (float)clipRect.x;
		float right = (float)clipRect.x + clipRect.width;
		float top = (float)clipRect.y;
		float bottom = (float)clipRect.y + clipRect.height;

		for(UINT32 i = 0; i < numQuads; i++)
		{
			UINT32 vertIdx = i * 4;

			// Attempt to skip those that are definitely not clipped
			if(vertices[vertIdx + 0].x >= left && vertices[vertIdx + 1].x <= right &&
				vertices[vertIdx + 0].y >= top && vertices[vertIdx + 2].y <= bottom)
			{
				continue;
			}

			// TODO - Skip those that are 100% clipped as well

			float du = (uv[vertIdx + 1].x - uv[vertIdx + 0].x) / (vertices[vertIdx + 1].x - vertices[vertIdx + 0].x);
			float dv = (uv[vertIdx + 0].y - uv[vertIdx + 2].y) / (vertices[vertIdx + 0].y - vertices[vertIdx + 2].y);

			// Clip left
			float newLeft = Math::Clamp(vertices[vertIdx + 0].x, left, right);
			float uvLeftOffset = (newLeft - vertices[vertIdx + 0].x) * du;

			// Clip right
			float newRight = Math::Clamp(vertices[vertIdx + 1].x, left, right);
			float uvRightOffset = (vertices[vertIdx + 1].x - newRight) * du;

			// Clip top
			float newTop = Math::Clamp(vertices[vertIdx + 0].y, top, bottom);
			float uvTopOffset = (newTop - vertices[vertIdx + 0].y) * dv;

			// Clip bottom
			float newBottom = Math::Clamp(vertices[vertIdx + 2].y, top, bottom);
			float uvBottomOffset = (vertices[vertIdx + 2].y - newBottom) * dv;

			vertices[vertIdx + 0].x = newLeft;
			vertices[vertIdx + 2].x = newLeft;
			vertices[vertIdx + 1].x = newRight;
			vertices[vertIdx + 3].x = newRight;
			vertices[vertIdx + 0].y = newTop;
			vertices[vertIdx + 1].y = newTop;
			vertices[vertIdx + 2].y = newBottom;
			vertices[vertIdx + 3].y = newBottom;
			
			if(uv != nullptr)
			{
				uv[vertIdx + 0].x += uvLeftOffset;
				uv[vertIdx + 2].x += uvLeftOffset;
				uv[vertIdx + 1].x -= uvRightOffset;
				uv[vertIdx + 3].x -= uvRightOffset;
				uv[vertIdx + 0].y += uvTopOffset;
				uv[vertIdx + 1].y += uvTopOffset;
				uv[vertIdx + 2].y -= uvBottomOffset;
				uv[vertIdx + 3].y -= uvBottomOffset;
			}
		}
	}
}