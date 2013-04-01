#include "CmTextSprite.h"
#include "CmVector2.h"
#include "CmGUIMaterialManager.h"

namespace CamelotEngine
{
	Sprite::Sprite()
		:mWidth(0), mHeight(0), mAnchor(SA_TopLeft), mIsDirty(true)
	{

	}

	Sprite::~Sprite()
	{
		clearMesh();
	}

	UINT32 Sprite::getNumRenderElements() const
	{
		if(mIsDirty)
		{
			updateMesh();
			mIsDirty = false;
		}

		return (UINT32)mCachedRenderElements.size();
	}

	const MaterialHandle& Sprite::getMaterial(UINT32 renderElementIdx) const
	{
		return mCachedRenderElements.at(renderElementIdx).material;
	}

	UINT32 Sprite::getNumQuads(UINT32 renderElementIdx) const
	{
		if(mIsDirty)
		{
			updateMesh();
			mIsDirty = false;
		}

		return mCachedRenderElements.at(renderElementIdx).numQuads;
	}

	UINT32 Sprite::fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const
	{
		if(mIsDirty)
		{
			updateMesh();
			mIsDirty = false;
		}

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

	Point Sprite::getAnchorOffset() const
	{
		switch(mAnchor)
		{
		case SA_TopLeft:
			return -Point(0, 0);
		case SA_TopCenter:
			return -Point(mWidth / 2, 0);
		case SA_TopRight:
			return -Point(mWidth, 0);
		case SA_MiddleLeft:
			return -Point(0, mHeight / 2);
		case SA_MiddleCenter:
			return -Point(mWidth / 2, mHeight / 2);
		case SA_MiddleRight:
			return -Point(mWidth, mHeight / 2);
		case SA_BottomLeft:
			return -Point(0, mHeight);
		case SA_BottomCenter:
			return -Point(mWidth / 2, mHeight);
		case SA_BottomRight:
			return -Point(mWidth, mHeight);
		}

		return Point();
	}

	bool Sprite::isClipRectangleValid() const
	{
		return mClipRect.width > 0 && mClipRect.height > 0;
	}

	void Sprite::clearMesh() const
	{
		for(auto& renderElem : mCachedRenderElements)
		{
			if(renderElem.vertices != nullptr)
				delete[] renderElem.vertices;

			if(renderElem.uvs != nullptr)
				delete[] renderElem.uvs;

			if(renderElem.indexes != nullptr)
				delete[] renderElem.indexes;

			if(renderElem.material != nullptr)
			{
				GUIMaterialManager::instance().releaseMaterial(renderElem.material);
			}
		}

		mCachedRenderElements.clear();
	}
}