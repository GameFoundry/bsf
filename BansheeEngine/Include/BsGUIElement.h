#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"
#include "BsGUIOptions.h"
#include "CmRectI.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIElement : public GUIElementBase
	{
	public:
		enum class ElementType
		{
			Label,
			Button,
			Toggle,
			Texture,
			InputBox,
			ListBox,
			ScrollArea,
			Layout,
			Undefined
		};

	public:
		GUIElement(GUIWidget& parent, const GUIElementStyle* style, const GUILayoutOptions& layoutOptions);
		virtual ~GUIElement();

		/**
		 * @brief	Returns the number of separate render elements in the GUI element.
		 * 			
		 * @note	GUI system attempts to reduce the number of GUI meshes so it will group
		 * 			sprites based on their material and textures. One render elements represents a group
		 * 			of such sprites that share a material/texture.
		 *
		 * @return	The number render elements.
		 */
		virtual CM::UINT32 getNumRenderElements() const = 0;

		/**
		 * @brief	Gets a material for the specified render element index.
		 * 			
		 * @see getNumRenderElements()
		 * 		
		 * @return	Handle to the material.
		 */
		virtual const GUIMaterialInfo& getMaterial(CM::UINT32 renderElementIdx) const = 0;

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
		virtual CM::UINT32 getNumQuads(CM::UINT32 renderElementIdx) const = 0;

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
		virtual void fillBuffer(CM::UINT8* vertices, CM::UINT8* uv, CM::UINT32* indices, CM::UINT32 startingQuad, 
			CM::UINT32 maxNumQuads, CM::UINT32 vertexStride, CM::UINT32 indexStride, CM::UINT32 renderElementIdx) const = 0;

		/**
		 * @brief	Recreates the internal render elements. Must be called before fillBuffer if element is dirty. 
		 * 			Marks the element as non dirty.
		 */
		void updateRenderElements();

		/**
		 * @brief	Gets non-clipped bounds that were assigned to the element by the parent layout.
		 */
		CM::RectI getBounds() const;

		/**
		 * @brief	Sets or removes focus from an element.
		 */
		void setFocus(bool enabled);

		virtual ElementType getElementType() const { return ElementType::Undefined; }

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool textInputEvent(const GUITextInputEvent& ev);
		virtual bool commandEvent(const GUICommandEvent& ev);
		virtual bool virtualButtonEvent(const GUIVirtualButtonEvent& ev);

		static void destroy(GUIElement* element);

		/************************************************************************/
		/* 							INTERNAL METHODS                      		*/
		/************************************************************************/

		void _setWidgetDepth(CM::UINT8 depth);
		void _setAreaDepth(CM::UINT16 depth);
		void _setOffset(const CM::Vector2I& offset);
		void _setWidth(CM::UINT32 width);
		void _setHeight(CM::UINT32 height);
		void _setClipRect(const CM::RectI& clipRect);
		virtual void _changeParentWidget(GUIWidget* widget);

		CM::UINT32 _getWidth() const { return mWidth; }
		CM::UINT32 _getHeight() const { return mHeight; }
		CM::Vector2I _getOffset() const { return mOffset; }
		virtual CM::UINT32 _getRenderElementDepth(CM::UINT32 renderElementIdx) const { return _getDepth(); }
		Type _getType() const { return GUIElementBase::Type::Element; }
		bool _isDestroyed() const { return mIsDestroyed; }

		const CM::RectI& _getClippedBounds() const { return mClippedBounds; }
		const CM::RectI& _getClipRect() const { return mClipRect; }
		const RectOffset& _getPadding() const;
		CM::UINT32 _getDepth() const { return mDepth; }
		GUIWidget& _getParentWidget() const { return *mParent; }
		virtual bool _isInBounds(const CM::Vector2I position) const;
		virtual bool _hasCustomCursor(const CM::Vector2I position, CursorType& type) const { return false; }
		virtual bool _acceptDragAndDrop(const CM::Vector2I position, CM::UINT32 typeId) const { return false; }

		virtual GUIContextMenu* getContextMenu() const { return nullptr; }

		/**
		 * @brief	Returns a clip rectangle relative to the element, used for offsetting
		 * 			the input text.
		 */
		virtual CM::Vector2I _getTextInputOffset() const { return CM::Vector2I(); }

		/**
		 * @brief	Returns a clip rectangle relative to the element, used for clipping
		 * 			the input text.
		 */
		virtual CM::RectI _getTextInputRect() const { return CM::RectI(); }

		const GUILayoutOptions& _getLayoutOptions() const { return mLayoutOptions; }
	protected:
		virtual void updateRenderElementsInternal();
		virtual void updateClippedBounds() = 0;

		void setLayoutOptions(const GUILayoutOptions& layoutOptions);
		
		CM::RectI getVisibleBounds() const;
		CM::RectI getContentBounds() const;
		CM::RectI getContentClipRect() const;

		GUIWidget* mParent;
		bool mIsDestroyed;
		GUILayoutOptions mLayoutOptions;
		CM::RectI mClippedBounds;

		CM::UINT32 mDepth;
		CM::Vector2I mOffset;
		CM::UINT32 mWidth, mHeight;
		CM::RectI mClipRect;
		const GUIElementStyle* mStyle;
	};
}