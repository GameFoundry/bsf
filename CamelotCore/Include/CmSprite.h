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

	struct SpriteRenderElement
	{
		SpriteRenderElement()
			:vertices(nullptr), uvs(nullptr), indexes(nullptr), numQuads(0)
		{ }

		Vector2* vertices;
		Vector2* uvs;
		UINT32* indexes;
		UINT32 numQuads;
		MaterialHandle material;
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

		/**
		 * @brief	Returns the number of separate render elements in the sprite. Normally this is one, but some sprites
		 * 			may consist of multiple materials, in which case each will require it's own mesh (render element)
		 * 			
		 * @return	The number render elements.
		 */
		UINT32 getNumRenderElements() const;

		/**
		 * @brief	Gets a material for the specified render element index.
		 * 			
		 * @see getNumRenderElements()
		 * 		
		 * @return	Handle to the material.
		 */
		const MaterialHandle& getMaterial(UINT32 renderElementIdx) const;

		/**
		 * @brief	Returns the number of quads that the specified render element will use. You will need this
		 * 			value when creating the buffers before calling "fillBuffer".
		 * 			
		 * @see getNumRenderElements()
		 * @see fillBuffer()
		 * 		
		 * @note	Number of vertices = Number of quads * 4
		 *			Number of indices = Number of quads * 6
		 *			
		 * @return	Number of quads for the specified render element. 
		 */
		UINT32 getNumQuads(UINT32 renderElementIdx) const;

		/**
		 * @brief	Fill the pre-allocated vertex, uv and index buffers with the mesh data for the specified render element.
		 *
		 * @see getNumRenderElements()
		 * @see	getNumQuads()
		 * 		
		 * @param   vertices			Previously allocated buffer where to store the vertices.
		 * @param   uv					Previously allocated buffer where to store the uv coordinates.
		 * @param   indices 			Previously allocated buffer where to store the indices.
		 * @param	startingQuad		At which quad should the method start filling the buffer.
		 * @param	maxNumQuads			Total number of quads the buffers were allocated for. Used only for memory safety.
		 * @param	renderElementIdx	Zero-based index of the render element.
		 */
		UINT32 fillBuffer(Vector2* vertices, Vector2* uv, UINT32* indices, UINT32 startingQuad, UINT32 maxNumQuads, UINT32 renderElementIdx) const;

	protected:
		Point mOffset;
		UINT32 mWidth, mHeight;
		Rect mClipRect;
		SpriteAnchor mAnchor;

		mutable bool mIsDirty;
		mutable vector<SpriteRenderElement>::type mCachedRenderElements;

		void setDirty() { mIsDirty = true; }
		Point getAnchorOffset() const;
		bool isClipRectangleValid() const;

		virtual void updateMesh() const = 0;
		void clearMesh() const;
	};
}