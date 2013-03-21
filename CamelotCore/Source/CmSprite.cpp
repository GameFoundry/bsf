#include "CmTextSprite.h"
#include "CmVector2.h"

namespace CamelotEngine
{
	Sprite::Sprite()
		:mWidth(0), mHeight(0), mAnchor(SA_TopLeft), mIsDirty(true),
		mVertices(nullptr), mUVs(nullptr), mIndexes(nullptr), mNumMeshQuads(0)
	{

	}

	Sprite::~Sprite()
	{

	}

	UINT32 Sprite::fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads)
	{
		if(mIsDirty)
		{
			updateMesh();
			mIsDirty = false;
		}

		UINT32 startVert = startingQuad * 4;
		UINT32 startIndex = startingQuad * 4;

		UINT32 maxVertIdx = maxNumQuads * 4;
		UINT32 maxIndexIdx = maxNumQuads * 6;

		UINT32 mNumVertices = mNumMeshQuads * 4;
		UINT32 mNumIndices = mNumMeshQuads * 6;

		assert((startVert + mNumVertices) <= maxVertIdx);
		assert((startIndex + mNumIndices) <= maxIndexIdx);

		memcpy(&vertices[startVert], mVertices, mNumVertices * sizeof(Vector2));
		memcpy(&uv[startVert], mUVs, mNumVertices * sizeof(Vector2));
		memcpy(&indices[startIndex], mIndexes, mNumIndices * sizeof(UINT32));

		return mNumMeshQuads;
	}

	UINT32 Sprite::getNumFaces()
	{
		if(mIsDirty)
		{
			updateMesh();
			mIsDirty = false;
		}

		return mNumMeshQuads;
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

	void Sprite::clearMesh()
	{
		if(mVertices != nullptr)
			delete[] mVertices;

		if(mUVs != nullptr)
			delete[] mUVs;

		if(mIndexes != nullptr)
			delete[] mIndexes;

		mNumMeshQuads = 0;
	}
}