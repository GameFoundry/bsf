#pragma once

#include "CmPrerequisites.h"
#include "CmPoint.h"
#include "CmRect.h"

namespace CamelotEngine
{
	enum SpriteAnchor
	{
		SA_TopLeft,
		SA_TopCenter,
		SA_TopRight,
		SA_MiddleLeft,
		SA_MiddleCenter,
		SA_MiddleRight,
		SA_BottomLeft,
		SA_BottomCenter,
		SA_BottomRight
	};

	class CM_EXPORT Sprite
	{
	public:
		Sprite();
		virtual ~Sprite();

		void setOffset(const Point& offset) { mOffset = offset; setDirty(); }
		void setSize(UINT32 width, UINT32 height) { mWidth = width; mHeight = height; setDirty(); }
		void setClipRect(const Rect& clipRect) { mClipRect = clipRect; setDirty(); }
		void setAnchor(SpriteAnchor anchor) { mAnchor = anchor; setDirty(); }

		Point getOffset() const { return mOffset; }
		UINT32 getWidth() const { return mWidth; }
		UINT32 getHeight() const { return mHeight; }
		Rect getClipRect() const { return mClipRect; }
		SpriteAnchor getAnchor() const { return mAnchor; }

		UINT32 fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads);

	protected:
		Point mOffset;
		UINT32 mWidth, mHeight;
		Rect mClipRect;
		SpriteAnchor mAnchor;

		bool mIsDirty;
		Vector2* mVertices;
		Vector2* mUVs;
		UINT32* mIndexes;
		UINT32 mNumMeshQuads;

		void setDirty() { mIsDirty = true; }
		Point getAnchorOffset() const;

		virtual void updateMesh() = 0;
		void clearMesh();
	};
}