#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "CmInt2.h"
#include "CmRect.h"

namespace BansheeEngine
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

		CM::Vector2* vertices;
		CM::Vector2* uvs;
		CM::UINT32* indexes;
		CM::UINT32 numQuads;
		GUIMaterialInfo matInfo;
	};

	class BS_EXPORT Sprite
	{
	public:
		Sprite();
		virtual ~Sprite();

		CM::Rect getBounds(const CM::Int2& offset, const CM::Rect& clipRect) const;

		/**
		 * @brief	Returns the number of separate render elements in the sprite. Normally this is one, but some sprites
		 * 			may consist of multiple materials, in which case each will require it's own mesh (render element)
		 * 			
		 * @return	The number render elements.
		 */
		CM::UINT32 getNumRenderElements() const;

		/**
		 * @brief	Gets a material for the specified render element index.
		 * 			
		 * @see getNumRenderElements()
		 * 		
		 * @return	Handle to the material.
		 */
		const GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const;

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
		CM::UINT32 getNumQuads(CM::UINT32 renderElementIdx) const;

		/**
		 * @brief	Fill the pre-allocated vertex, uv and index buffers with the mesh data for the
		 * 			specified render element.
		 * 			
		 * @see getNumRenderElements()
		 * @see	getNumQuads()
		 *
		 * @param	vertices			Previously allocated buffer where to store the vertices.
		 * @param	uv					Previously allocated buffer where to store the uv coordinates.
		 * @param	indices				Previously allocated buffer where to store the indices.
		 * @param	startingQuad		At which quad should the method start filling the buffer.
		 * @param	maxNumQuads			Total number of quads the buffers were allocated for. Used only
		 * 								for memory safety.
		 * @param	vertexStride		Number of bytes between of vertices in the provided vertex and uv data.
		 * @param	indexStride			Number of bytes between two indexes in the provided index data.
		 * @param	renderElementIdx	Zero-based index of the render element.
		 */
		CM::UINT32 fillBuffer(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32* indices, CM::UINT32 startingQuad, CM::UINT32 maxNumQuads, 
			CM::UINT32 vertexStride, CM::UINT32 indexStride, CM::UINT32 renderElementIdx, const CM::Int2& offset, const CM::Rect& clipRect) const;

		static void clipToRect(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32 numQuads, CM::UINT32 vertStride, const CM::Rect& clipRect);
		static CM::Int2 getAnchorOffset(SpriteAnchor anchor, CM::UINT32 width, CM::UINT32 height);
	protected:
		mutable CM::Rect mBounds;
		mutable CM::Vector<SpriteRenderElement>::type mCachedRenderElements;

		void updateBounds() const;
		void clearMesh() const;
	};
}