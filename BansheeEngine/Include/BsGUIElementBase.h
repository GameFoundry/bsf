#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "BsGUIDimensions.h"
#include "BsRect2I.h"
#include "BsVector2I.h"
#include "BsRectOffset.h"

namespace BansheeEngine
{
	/**
	 * @brief	Base class for all GUI elements (visible or layout).
	 */
	class BS_EXPORT GUIElementBase
	{
	public:
		/**
		 * @brief	Valid types of GUI base elements.
		 */
		enum class Type
		{
			Layout,
			Element,
			FixedSpace,
			FlexibleSpace,
			Panel
		};

	public:
		GUIElementBase();
		GUIElementBase(const GUIDimensions& dimensions);
		virtual ~GUIElementBase();

		/**
		 * @brief	Sets element position relative to parent GUI panel.
		 *
		 * @note	Be aware that this value will be ignored if GUI element is part of a layout since then
		 *			the layout controls its placement.
		 */
		void setPosition(INT32 x, INT32 y);

		/**
		 * @brief	Sets element width in pixels. 
		 */
		void setWidth(UINT32 width);

		/**
		 * @brief	Sets element width in pixels. Element will be resized according to its 
		 *			contents and parent layout but will always stay within the provided range.
		 *			If maximum width is zero, the element is allowed to expand as much as it needs.
		 */
		void setFlexibleWidth(UINT32 minWidth = 0, UINT32 maxWidth = 0);

		/**
		 * @brief	Sets element height in pixels.
		 */
		void setHeight(UINT32 height);

		/**
		 * @brief	Sets element height in pixels. Element will be resized according to its 
		 *			contents and parent layout but will always stay within the provided range.
		 *			If maximum height is zero, the element is allowed to expand as much as it needs.
		 */
		void setFlexibleHeight(UINT32 minHeight = 0, UINT32 maxHeight = 0);

		/**
		 * @brief	Resets element dimensions to their initial values dictated by the elements style.
		 */
		virtual void resetDimensions();

		/**
		 * @brief	Enables (default) this element and all its children.
		 */
		void enableRecursively();

		/**
		 * @brief	Disables this element and all its children.
		 */
		void disableRecursively();

		/**
		 * @brief	Returns non-clipped bounds of the GUI element. Relative to the parent GUI panel.
		 *
		 * @note	This call can be potentially expensive as the bounds need to be calculated based on current GUI state.
		 */
		virtual Rect2I getBounds() const;

		/**
		 * @brief	Returns non-clipped visible bounds of the GUI element (bounds exclude the margins). Relative to the parent GUI panel.
		 *
		 * @note	This call can be potentially expensive as the bounds need to be calculated based on current GUI state.
		 */
		virtual Rect2I getVisibleBounds() const;

		/************************************************************************/
		/* 							INTERNAL METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Updates child elements positions, sizes, clip rectanges and depths so they
		 *			fit into the provided bounds, while respecting their layout options. 
		 *
		 * @param	x			X position of the area to start laying out the elements. Relative to parent widget.
		 * @param	y			Y position of the area to start laying out the elements. Relative to parent widget.
		 * @param	width		Width of the area to lay out the elements, in pixels.
		 * @param	height		Height of the area to lay out the elements, in pixels.
		 * @param	clipRect	Rectangle to use for clipping of GUI elements. Any element outside of this rectangle will have its
		 *						visible geometry clipped. In coordinates relative to parent widget.
		 * @param	widgetDepth	Depth of the parent widget, will be set for all child elements.
		 * @param	areaDepth	Depth of the parent area, will be set for all child elements.
		 *
		 * @note	Internal method.
		 */
		virtual void _updateLayout(INT32 x, INT32 y, UINT32 width, UINT32 height, 
			Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		/**
		 * @brief	Calculates optimal sizes of all child elements, as determined by their style and layout options.
		 *
		 * @note	Internal method.
		 */
		virtual void _updateOptimalLayoutSizes();

		/**
		 * @copydoc	_updateLayout
		 *
		 * @note	Internal method.
		 */
		virtual void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		/**
		 * @brief	Calculates positions & sizes of all elements in the layout. This method expects a pre-allocated array to store the data in.
		 *
		 * @brief	x				Start X coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @brief	y				Start Y coordinate of the layout area. First element will be placed here. Relative to parent widget.
		 * @param	width			Available width for the layout elements.
		 * @param	height			Available height for the layout elements.
		 * @param	elementAreas	Array to hold output areas. Must be the same size as the number of child elements.
		 * @param	numElements		Size of the element areas array.
		 * @param	sizeRanges		Ranges of possible sizes used for the child elements. Array must be same size as elements array.
		 * @param	mySizeRange		Size range of this element.
		 */
		virtual void _getElementAreas(INT32 x, INT32 y, UINT32 width, UINT32 height, Rect2I* elementAreas, UINT32 numElements, 
			const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const;

		/**
		 * @brief	Sets a new parent for this element.
		 *
		 * @note	Internal method.
		 */
		void _setParent(GUIElementBase* parent);

		/**
		 * @brief	Returns number of child elements.
		 *
		 * @note	Internal method.
		 */
		UINT32 _getNumChildren() const { return (UINT32)mChildren.size(); }

		/**
		 * @brief	Return the child element at the specified index.
		 *
		 * @note	Internal method.
		 */
		GUIElementBase* _getChild(UINT32 idx) const { return mChildren[idx]; }

		/**
		 * @brief	Returns previously calculated optimal size for this element.
		 *
		 * @note	Internal method.
		 */
		virtual Vector2I _getOptimalSize() const = 0;

		/**
		 * @brief	Returns layout options that determine how is the element positioned and sized.
		 *
		 * @note	Internal method.
		 */
		const GUIDimensions& _getDimensions() const { return mDimensions; }

		/**
		 * @brief	Returns element size range constrained by its layout options.
		 *
		 * @note	Internal method.
		 */
		virtual LayoutSizeRange _calculateLayoutSizeRange() const ;

		/**
		 * @brief	Returns element padding that determines how far apart to space out this element
		 *			from other elements in a layout.
		 *
		 * @note	Internal method.
		 */
		virtual const RectOffset& _getPadding() const = 0;

		/**
		 * @brief	Returns specific sub-type of this object.
		 *
		 * @note	Internal method.
		 */
		virtual Type _getType() const = 0;

		/**
		 * @brief	Returns parent GUI base element.
		 *
		 * @note	Internal method.
		 */
		GUIElementBase* _getParent() const { return mParentElement; }

		/**
		 * @brief	Returns parent GUI widget, can be null.
		 *
		 * @note	Internal method.
		 */
		GUIWidget* _getParentWidget() const { return mParentWidget; }

		/**
		 * @brief	Marks the internal data as clean, usually called after an external
		 *			system has registered the internal data change.
		 *
		 * @note	Internal method.
		 */
		void _markAsClean() { mIsDirty = 0; }

		/**
		 * @brief	Returns true if render elements need to be recalculated by calling updateRenderElements.
		 *
		 * @note	Internal method.
		 */
		virtual bool _isContentDirty() const;

		/**
		 * @brief	Returns true if mesh has changed and fillBuffers needs to be called.
		 *
		 * @note	Internal method.
		 */
		virtual bool _isMeshDirty() const; 

		/**
		 * @brief	Returns true if element is disabled and won't be visible or interactable.
		 *
		 * @note	Internal method.
		 */
		bool _isDisabled() const { return mIsDisabled; }

		/**
		 * @brief	Changes the active GUI element widget. This allows you to move an element
		 *			to a different viewport, or change element style by using a widget with a different skin.
		 *			You are allowed to pass null here, but elements with no parent will be unmanaged. You will be
		 *			responsible for deleting them manually, and they will not render anywhere.
		 *
		 * @note	Internal method.
		 */
		virtual void _changeParentWidget(GUIWidget* widget);

		/**
		 * @brief	Registers a new child element.
		 *
		 * @note	Internal method.
		 */
		void _registerChildElement(GUIElementBase* element);

		/**
		 * @brief	Unregisters an existing child element.
		 *
		 * @note	Internal method.
		 */
		void _unregisterChildElement(GUIElementBase* element);

	protected:
		/**
		 * @brief	Marks the elements contents as dirty, which causes the sprite meshes to be recreated from scratch.
		 */
		void markContentAsDirty();

		/**
		 * @brief	Mark only the elements that operate directly on the sprite mesh without requiring the mesh
		 * 			to be recreated as dirty. This includes position, depth and clip rectangle.
		 */
		void markMeshAsDirty();

		GUIWidget* mParentWidget;
		GUIElementBase* mParentElement;
		Vector<GUIElementBase*> mChildren;	
		UINT8 mIsDirty;
		bool mIsDisabled;

		GUIDimensions mDimensions;
	};
}