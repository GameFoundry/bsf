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

	UINT32 Sprite::fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const
	{
		auto renderElem = mCachedRenderElements.at(renderElementIdx);

		UINT32 startVert = startingQuad * 4;
		UINT32 startIndex = startingQuad * 4;

		UINT32 maxVertIdx = maxNumQuads * 4;
		UINT32 maxIndexIdx = maxNumQuads * 6;

		UINT32 mNumVertices = renderElem.numQuads * 4;
		UINT32 mNumIndices = renderElem.numQuads * 6;

		assert((startVert + mNumVertices) <= maxVertIdx);
		assert((startIndex + mNumIndices) <= maxIndexIdx);

		memcpy(&vertices[startVert], renderElem.vertices, mNumVertices * sizeof(Vector2));
		memcpy(&uv[startVert], renderElem.uvs, mNumVertices * sizeof(Vector2));
		memcpy(&indices[startIndex], renderElem.indexes, mNumIndices * sizeof(UINT32));

		return renderElem.numQuads;
	}

	Point Sprite::getAnchorOffset(SpriteAnchor anchor, UINT32 width, UINT32 height) const
	{
		switch(anchor)
		{
		case SA_TopLeft:
			return -Point(0, 0);
		case SA_TopCenter:
			return -Point(width / 2, 0);
		case SA_TopRight:
			return -Point(width, 0);
		case SA_MiddleLeft:
			return -Point(0, height / 2);
		case SA_MiddleCenter:
			return -Point(width / 2, height / 2);
		case SA_MiddleRight:
			return -Point(width, height / 2);
		case SA_BottomLeft:
			return -Point(0, height);
		case SA_BottomCenter:
			return -Point(width / 2, height);
		case SA_BottomRight:
			return -Point(width, height);
		}

		return Point();
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
				CM_DELETE_ARRAY(renderElem.vertices, Vector2, vertexCount, ScratchAlloc);

			if(renderElem.uvs != nullptr)
				CM_DELETE_ARRAY(renderElem.uvs, Vector2, vertexCount, ScratchAlloc);

			if(renderElem.indexes != nullptr)
				CM_DELETE_ARRAY(renderElem.indexes, UINT32, indexCount, ScratchAlloc);

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
	void Sprite::clipToRect(Vector2* vertices, Vector2* uv, UINT32 numQuads, const Rect& clipRect) const
	{
		float left = (float)clipRect.x;
		float right = (float)clipRect.x + clipRect.width;
		float top = (float)clipRect.y;
		float bottom = (float)clipRect.y - clipRect.height;

		for(UINT32 i = 0; i < numQuads; i++)
		{
			UINT32 vertIdx = i * 4;

			// Attempt to skip those that are definitely not clipped
			if(vertices[vertIdx + 0].x >= left && vertices[vertIdx + 1].x <= right &&
				vertices[vertIdx + 0].y <= top && vertices[vertIdx + 2].y >= bottom)
			{
				continue;
			}

			float du = (uv[vertIdx + 1].x - uv[vertIdx + 0].x) / (vertices[vertIdx + 1].x - vertices[vertIdx + 0].x);
			float dv = (uv[vertIdx + 0].y - uv[vertIdx + 2].y) / (vertices[vertIdx + 0].y - vertices[vertIdx + 2].y);

			// Clip left
			float newLeft = Math::Clamp(vertices[vertIdx + 0].x, left, right);
			float uvLeftOffset = (newLeft - vertices[vertIdx + 0].x) * du;

			vertices[vertIdx + 0].x = newLeft;
			vertices[vertIdx + 2].x = newLeft;
			uv[vertIdx + 0].x += uvLeftOffset;
			uv[vertIdx + 2].x += uvLeftOffset;

			// Clip right
			float newRight = Math::Clamp(vertices[vertIdx + 1].x, left, right);
			float uvRightOffset = (vertices[vertIdx + 1].x - newRight) * du;

			vertices[vertIdx + 1].x = newRight;
			vertices[vertIdx + 3].x = newRight;
			uv[vertIdx + 1].x -= uvRightOffset;
			uv[vertIdx + 3].x -= uvRightOffset;

			// Clip top
			float newTop = Math::Clamp(vertices[vertIdx + 0].y, bottom, top);
			float uvTopOffset = (vertices[vertIdx + 0].y - newTop) * dv;

			vertices[vertIdx + 0].y = newTop;
			vertices[vertIdx + 1].y = newTop;
			uv[vertIdx + 0].y -= uvTopOffset;
			uv[vertIdx + 1].y -= uvTopOffset;

			// Clip bottom
			float newBottom = Math::Clamp(vertices[vertIdx + 2].y, bottom, top);
			float uvBottomOffset = (newBottom - vertices[vertIdx + 2].y) * dv;

			vertices[vertIdx + 2].y = newBottom;
			vertices[vertIdx + 3].y = newBottom;
			uv[vertIdx + 2].y += uvBottomOffset;
			uv[vertIdx + 3].y += uvBottomOffset;
		}
	}
}