#pragma once

#include "BsPrerequisites.h"
#include "BsGUILayoutOptions.h"
#include "CmRect.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIElement
	{
	public:
		GUIElement(GUIWidget& parent, const GUI_LAYOUT_OPTIONS& layoutOptions);

		/**
		 * @brief	Returns the number of separate render elements in the GUI element.
		 * 			
		 * @note	GUI system attempts to reduce the number of GUI meshes so it will group
		 * 			sprites based on their material and textures. One render elements represents a group
		 * 			of such sprites that share a material/texture.
		 *
		 * @return	The number render elements.
		 */
		virtual UINT32 getNumRenderElements() const = 0;

		/**
		 * @brief	Gets a material for the specified render element index.
		 * 			
		 * @see getNumRenderElements()
		 * 		
		 * @return	Handle to the material.
		 */
		virtual const CM::HMaterial& getMaterial(UINT32 renderElementIdx) const = 0;

		/**
		 * @brief	Returns the number of quads that the specified render element will use. You will need this
		 * 			value when creating the buffers before calling "fillBuffer.
		 * 			
		 * @see getNumRenderElements()
		 * @see fillBuffer()
		 * 		
		 * @note	Number of vertices = Number of quads * 4
		 *			Number of indices = Number of quads * 6
		 *			
		 * @return	Number of quads for the specified render element. 
		 */
		virtual UINT32 getNumQuads(UINT32 renderElementIdx) const = 0;

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
		virtual void fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const = 0;

		/**
		 * @brief	Recreates the internal render elements. Must be called before fillBuffer if element is dirty. 
		 * 			Marks the element as non dirty.
		 */
		void updateRenderElements();

		virtual bool mouseEvent(const GUIMouseEvent& ev);

		const CM::Rect& getBounds() const { return mBounds; }
		INT32 getDepth() const { return mDepth; }
		bool isDirty() const { return mIsDirty; }

		static void destroy(GUIElement* element);

		//  onMouseMove
		//	onMousePress
		//	onMouseReleased
		//	onKeyPressed
		//	onKeyReleased
	protected:
		friend class GUIWidget;
		friend class GUILayout;
		friend class GUILayoutX;
		friend class GUILayoutY;

		virtual ~GUIElement();

		virtual void updateRenderElementsInternal() = 0;

		GUILayout* getParentLayout() const { return mParentLayout; }
		void setParentLayout(GUILayout* layout) { mParentLayout = layout; }

		void setDepth(INT32 depth) { mDepth = depth; }
		void setOffset(const CM::Int2& offset) { mOffset = offset; }
		void setWidth(UINT32 width) { mWidth = width; }
		void setHeight(UINT32 height) { mHeight = height; }
		void setClipRect(const CM::Rect& clipRect) { mClipRect = clipRect; }

		UINT32 getWidth() const { return mWidth; }
		UINT32 getHeight() const { return mHeight; }

		void setLayoutOptions(const GUI_LAYOUT_OPTIONS& layoutOptions) { mLayoutOptions = layoutOptions; }
		const GUI_LAYOUT_OPTIONS& getLayoutOptions() const { return mLayoutOptions; }

		void markAsClean() { mIsDirty = false; }
		void markAsDirty() { mIsDirty = true; }

		virtual UINT32 getOptimalWidth() const = 0;
		virtual UINT32 getOptimalHeight() const = 0;

		GUIWidget& mParent;
		GUILayout* mParentLayout;
		GUI_LAYOUT_OPTIONS mLayoutOptions;
		CM::Rect mBounds;

		bool mIsDirty;
		INT32 mDepth;
		CM::Int2 mOffset;
		UINT32 mWidth, mHeight;
		CM::Rect mClipRect;
		const GUIElementStyle* mStyle;

		static void destroyInternal(GUIElement* element);
	};
}