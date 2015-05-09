#pragma once

#include "BsPrerequisites.h"
#include "BsGUIElementBase.h"
#include "BsGUIOptions.h"
#include "BsRect2I.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	/**
	 * Represents parent class for all visible GUI elements. Contains methods
	 * needed for positioning, rendering and handling input.
	 */
	class BS_EXPORT GUIElement : public GUIElementBase
	{
	public:
		/**
		 * @brief	Different sub-types of GUI elements.
		 */
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
		GUIElement(const String& styleName, const GUIDimensions& dimensions);
		virtual ~GUIElement();

		/**
		 * @brief	Sets or removes focus from an element. Will change element style.
		 */
		void setFocus(bool enabled);

		/**
		 * @brief	Sets the tint of the GUI element.
		 */
		virtual void setTint(const Color& color) { }

		/**
		 * @copydoc	GUIElementBase::resetDimensions
		 */
		virtual void resetDimensions() override;

		/**
		 * @brief	Sets new style to be used by the element.
		 */
		void setStyle(const String& styleName);

		/**
		 * @copydoc	GUIElementBase::getVisibleBounds
		 */
		Rect2I getVisibleBounds() override;

		/**
		 * @brief	Destroy the element. Removes it from parent and widget, and queues
		 *			it for deletion. Element memory will be released delayed, next frame.
		 */	
		static void destroy(GUIElement* element);

		/************************************************************************/
		/* 							INTERNAL METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Returns the number of separate render elements in the GUI element.
		 * 			
		 * @return	The number render elements.
		 *
		 * @note	Internal method.
		 *			GUI system attempts to reduce the number of GUI meshes so it will group
		 * 			sprites based on their material and textures. One render elements represents a group
		 * 			of such sprites that share a material/texture.
		 */
		virtual UINT32 _getNumRenderElements() const = 0;

		/**
		 * @brief	Gets a material for the specified render element index.
		 * 		
		 * @return	Handle to the material.
		 *
		 * @see		getNumRenderElements()
		 *
		 * @note	Internal method.
		 */
		virtual const GUIMaterialInfo& _getMaterial(UINT32 renderElementIdx) const = 0;

		/**
		 * @brief	Returns the number of quads that the specified render element will use. You will need this
		 * 			value when creating the buffers before calling "fillBuffer.
		 * 			
		 * @return	Number of quads for the specified render element. 
		 *
		 * @see		getNumRenderElements()
		 * @see		fillBuffer()
		 * 		
		 * @note	Internal method.
		 *			Number of vertices = Number of quads * 4
		 *			Number of indices = Number of quads * 6	
		 */
		virtual UINT32 _getNumQuads(UINT32 renderElementIdx) const = 0;

		/**
		 * @brief	Fill the pre-allocated vertex, uv and index buffers with the mesh data for the
		 * 			specified render element.
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
		 *
		 * @see		getNumRenderElements()
		 * @see		getNumQuads()
		 *
		 * @note	Internal method.
		 */
		virtual void _fillBuffer(UINT8* vertices, UINT8* uv, UINT32* indices, UINT32 startingQuad, 
			UINT32 maxNumQuads, UINT32 vertexStride, UINT32 indexStride, UINT32 renderElementIdx) const = 0;

		/**
		 * @brief	Recreates the internal render elements. Must be called before fillBuffer if element is dirty. 
		 * 			Marks the element as non dirty.
		 *
		 * @note	Internal method.
		 */
		void _updateRenderElements();

		/**
		 * @brief	Gets internal element style representing the exact type of GUI element
		 *			in this object.
		 *
		 * @note	Internal method.
		 */
		virtual ElementType _getElementType() const { return ElementType::Undefined; }

		/**
		 * @brief	Called when a mouse event is received on any GUI element the mouse is interacting
		 *			with. Return true if you have processed the event and don't want other elements to process it.
		 *
		 * @note	Internal method.
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev);

		/**
		 * @brief	Called when some text is input and the GUI element has input focus. 
		 *			Return true if you have processed the event and don't want other elements to process it.
		 *
		 * @note	Internal method.
		 */	
		virtual bool _textInputEvent(const GUITextInputEvent& ev);

		/**
		 * @brief	Called when a command event is triggered. Return true if you have processed the event and 
		 *			don't want other elements to process it.
		 *
		 * @note	Internal method.
		 */
		virtual bool _commandEvent(const GUICommandEvent& ev);

		/**
		 * @brief	Called when a virtual button is pressed/released and the GUI element has input focus. 
		 *			Return true if you have processed the event and don't want other elements to process it.
		 *
		 * @note	Internal method.
		 */
		virtual bool _virtualButtonEvent(const GUIVirtualButtonEvent& ev);

		/**
		 * @brief	Set element part of element depth. Less significant than both
		 *			widget and area depth.
		 *
		 * @note	Internal method.
		 */
		void _setElementDepth(UINT8 depth);

		/**
		 * @copydoc	GUIElementBase::_setLayoutData
		 */
		virtual void _setLayoutData(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElementBase::_changeParentWidget
		 */
		virtual void _changeParentWidget(GUIWidget* widget);

		/**
		 * @brief	Returns depth for a specific render element. This contains a combination
		 *			of widget depth (8 bit(, area depth (16 bit) and render element depth (8 bit)
		 *
		 * @note	Internal method.
		 *
		 * @see		getNumRenderElements
		 */
		virtual UINT32 _getRenderElementDepth(UINT32 renderElementIdx) const { return _getDepth(); }

		/**
		 * @brief	Returns the range of depths that the child elements can be rendered it.
		 *
		 * @note	e.g. if you are rendering a button with an image and a text you will want the text
		 *			to be rendered in front of the image at a different depth, which means the depth
		 *			range is 2 (0 for text, 1 for background image).
		 */
		virtual UINT32 _getRenderElementDepthRange() const { return 1; }

		/**
		 * @brief	Gets internal element style representing the exact type of GUI element
		 *			in this object.
		 *
		 * @note	Internal method.
		 */
		Type _getType() const { return GUIElementBase::Type::Element; }

		/**
		 * @brief	Checks if element has been destroyed and is queued for deletion.
		 *
		 * @note	Internal method.
		 */
		bool _isDestroyed() const { return mIsDestroyed; }

		/**
		 * @brief	Update element style based on active GUI skin and style name.
		 *
		 * @note	Internal method.
		 */
		void _refreshStyle();

		/**
		 * @brief	Gets the currently active element style.
		 *
		 * @note	Internal method.
		 */
		const GUIElementStyle* _getStyle() const { return mStyle; }

		/**
		 * @brief	Gets GUI element bounds relative to parent widget, clipped by specified clip rect.
		 *
		 * @note	Internal method.
		 */
		const Rect2I& _getClippedBounds() const { return mClippedBounds; }

		/**
		 * @brief	Returns GUI element padding. Padding is modified by changing element style and determines
		 *			minimum distance between different GUI elements.
		 *
		 * @note	Internal method.
		 */
		const RectOffset& _getPadding() const;

		/**
		 * @brief	Returns GUI element depth. This includes widget and area depth, but does not
		 *			include specific per-render-element depth.
		 *
		 * @note	Internal method.
		 */
		UINT32 _getDepth() const { return mLayoutData.depth; }

		/**
		 * @brief	Checks is the specified position within GUI element bounds. Position is relative to
		 *			parent GUI widget.
		 *
		 * @note	Internal method.
		 */
		virtual bool _isInBounds(const Vector2I position) const;

		/**
		 * @brief	Checks if the GUI element has a custom cursor and outputs the cursor type if it does.
		 *
		 * @note	Internal method.
		 */
		virtual bool _hasCustomCursor(const Vector2I position, CursorType& type) const { return false; }

		/**
		 * @brief	Checks if the GUI element accepts a drag and drop operation of the specified type.
		 *
		 * @note	Internal method.
		 */
		virtual bool _acceptDragAndDrop(const Vector2I position, UINT32 typeId) const { return false; }

		/**
		 * @brief	Returns a context menu if a GUI element has one. Otherwise returns nullptr.
		 *
		 * @note	Internal method.
		 */
		virtual GUIContextMenu* _getContextMenu() const { return nullptr; }

		/**
		 * @brief	Returns a clip rectangle relative to the element, used for offsetting
		 * 			the input text.
		 *
		 * @note	Internal method.
		 */
		virtual Vector2I _getTextInputOffset() const { return Vector2I(); }

		/**
		 * @brief	Returns a clip rectangle relative to the element, used for clipping
		 * 			the input text.
		 *
		 * @note	Internal method.
		 */
		virtual Rect2I _getTextInputRect() const { return Rect2I(); }

	protected:
		/**
		 * @brief	Called whenever render elements are dirty and need to be rebuilt.
		 */
		virtual void updateRenderElementsInternal();

		/**
		 * @brief	Called whenever element clipped bounds need to be recalculated. (e.g. when
		 *			width, height or clip rectangles changes).
		 */
		virtual void updateClippedBounds() = 0;

		/**
		 * @brief	Helper method that returns style name used by an element of a certain type.
		 *			If override style is empty, default style for that type is returned.
		 */
		template<class T>
		static const String& getStyleName(const String& overrideStyle)
		{
			if(overrideStyle == StringUtil::BLANK)
				return T::getGUITypeName();

			return overrideStyle;
		}

		/**
		 * @brief	Attempts to find a sub-style for the specified type in the currently
		 *			set GUI element style. If one cannot be found empty string is returned.
		 */
		const String& getSubStyleName(const String& subStyleTypeName) const;

		/**
		 * @brief	Returns the name of the style used by this element.
		 */
		const String& getStyleName() const { return mStyleName; }

		/**
		 * @brief	Method that gets triggered whenever element style changes.
		 */
		virtual void styleUpdated() { }

		/**
		 * @brief	Returns clipped bounds excluding the margins. Relative to parent widget.
		 */
		Rect2I getCachedVisibleBounds() const;

		/**
		 * @brief	Returns bounds of the content contained within the GUI element. Relative to parent widget.
		 */
		Rect2I getCachedContentBounds() const;

		/**
		 * @brief	Returns a clip rectangle that can be used for clipping the contents of this
		 *			GUI element. Clip rect is relative to GUI element origin.
		 */
		Rect2I getCachedContentClipRect() const;

		bool mIsDestroyed;
		Rect2I mClippedBounds;

	private:
		const GUIElementStyle* mStyle;
		String mStyleName;
	};
}