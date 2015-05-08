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
		 * @param	x					X position of the area to start laying out the elements. Relative to parent widget.
		 * @param	y					Y position of the area to start laying out the elements. Relative to parent widget.
		 * @param	width				Width of the area to lay out the elements, in pixels.
		 * @param	height				Height of the area to lay out the elements, in pixels.
		 * @param	clipRect			Rectangle to use for clipping of GUI elements. Any element outside of this rectangle will have its
		 *								visible geometry clipped. In coordinates relative to parent widget.
		 * @param	widgetDepth			Depth of the parent widget, will be set for all child elements.
		 * @param	panelDepth			Depth of the parent panel, will be set for all child elements.
		 * @param	panelDepthRangeMin  Minimum depth range that child GUI panels can have (relative to panelDepth).
		 *								Values outside of the depth range will be clamped.
		 * @param	panelDepthRangeMax	Maximum depth range that child GUI panels can have (relative to panelDepth).
		 *								Values outside of the depth range will be clamped.
		 *
		 * @note	Internal method.
		 */
		virtual void _updateLayout(INT32 x, INT32 y, UINT32 width, UINT32 height, 
			Rect2I clipRect, UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax);

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
			Rect2I clipRect, UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax);

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
		 * @brief	Sets element position relative to widget origin. This will be the position used directly for rendering.
		 *
		 * @note	Internal method.
		 */
		virtual void _setPosition(const Vector2I& offset);

		/**
		 * @brief	Sets element width in pixels. This will be the width used directly for rendering.
		 *
		 * @note	Internal method.
		 */
		virtual void _setWidth(UINT32 width);

		/**
		 * @brief	Sets element height in pixels. This will be the height used directly for rendering.
		 *
		 * @note	Internal method.
		 */
		virtual void _setHeight(UINT32 height);

		/**
		 * @brief	Set widget part of element depth. (Most significant part)
		 *
		 * @note	Internal method.
		 */
		void _setWidgetDepth(UINT8 depth);

		/**
		 * @brief	Set area part of element depth. Less significant than widget
		 *			depth but more than custom element depth.
		 *
		 * @note	Internal method.
		 */
		void _setAreaDepth(INT16 depth);

		/**
		 * @brief	Sets a clip rectangle that GUI element sprite will be clipped to. 
		 *			Rectangle is in local coordinates. (Relative to element position)
		 *
		 * @note	Internal method.
		 */
		virtual void _setClipRect(const Rect2I& clipRect);

		/**
		 * @brief	Sets the panel depth range that children of this element are allowed
		 *			to be placed in.
		 */
		void _setPanelDepthRange(UINT16 min, UINT16 max);

		/**
		 * @brief	Retrieves the panel depth range that children of this element are allowed
		 *			to be placed in.
		 */
		void _getPanelDepthRange(UINT16& min, UINT16& max);

		/**
		 * @brief	Returns width of the element in pixels.
		 *
		 * @note	This value is updated during layout update which means it might be out of date
		 *			if parent element bounds changed since.
		 *			Internal method:
		 */
		UINT32 _getWidth() const { return mWidth; }

		/**
		 * @brief	Returns height of the element in pixels.
		 *
		 * @note	This value is updated during layout update which means it might be out of date
		 *			if parent element bounds changed since.
		 *			Internal method:
		 */
		UINT32 _getHeight() const { return mHeight; }

		/**
		 * @brief	Returns position of the element, relative to parent GUI widget origin.
		 *
		 * @note	This value is updated during layout update which means it might be out of date
		 *			if parent element bounds changed since.
		 *			Internal method:
		 */
		Vector2I _getOffset() const { return mOffset; }

		/**
		 * @brief	Set widget part of element depth. (Most significant part)
		 *
		 * @note	Internal method.
		 */
		UINT8 _getWidgetDepth() const;

		/**
		 * @brief	Set area part of element depth. Less significant than widget
		 *			depth but more than custom element depth.
		 *
		 * @note	Internal method.
		 */
		INT16 _getAreaDepth() const;

		/**
		 * @brief	Returns clip rect used for clipping the GUI element and related sprites
		 *			to a specific region. Clip rect is relative to GUI element origin.
		 *
		 * @note	Internal method.
		 */
		const Rect2I& _getClipRect() const { return mClipRect; }

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
		 * @brief	Returns the parent element whose layout needs to be updated 
		 *			when this elements contents change.
		 *
		 * @note	Due to the nature of the GUI system, when a child element bounds
		 *			or contents change, its parents and siblings usually need their
		 *			layout bound updated. This function returns the first parent of 
		 *			all the elements that require updating. This parent usually
		 *			has fixed bounds or some other property that allows its children
		 *			to be updated independently from the even higher-up elements.
		 */
		GUIElementBase* _getUpdateParent() const { return mUpdateParent; }

		/**
		 * @brief	Returns parent GUI widget, can be null.
		 *
		 * @note	Internal method.
		 */
		GUIWidget* _getParentWidget() const { return mParentWidget; }

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

		/**
		 * @brief	Marks the elements contents as dirty, which causes the sprite meshes to be recreated from scratch.
		 *
		 * @note	Internal method.
		 */
		void markContentAsDirty();

		/**
		 * @brief	Mark only the elements that operate directly on the sprite mesh without requiring the mesh
		 * 			to be recreated as dirty. This includes position, depth and clip rectangle.
		 *
		 * @note	Internal method.
		 */
		void markMeshAsDirty();

		/**
		 * @brief	Returns true if elements contents have changed since last update.
		 */
		bool _isDirty() const { return mIsDirty; }

		/**
		 * @brief	Marks the element contents to be up to date. (i.e. processed by the GUI system)
		 */
		void _markAsClean();

	protected:

		/**
		 * @brief	Finds anchor and update parents and recursively assigns them to all children.
		 */
		void _updateAUParents();

		/**
		 * @brief	Refreshes update parents of all child elements.
		 */
		void refreshChildUpdateParents();

		/**
		 * @brief	Finds the first parent element whose size doesn't depend on child sizes.
		 *			
		 * @note	This allows us to optimize layout updates and trigger them only on such parents
		 *			when their child elements contents change, compared to doing them on the entire
		 *			GUI hierarchy.
		 */
		GUIElementBase* findUpdateParent();

		/**
		 * @brief	Helper method for recursion in "_updateAUParents". Sets the provided anchor 
		 *			parent for all children recursively. Recursion stops when a child anchor is detected.
		 *
		 * @see	_updateParents
		 */
		void setAnchorParent(GUIPanel* anchorParent);

		/**
		 * @brief	Helper method for recursion in "_updateAUParents". Sets the provided update 
		 *			parent for all children recursively. Recursion stops when a child update parent
		 *			is detected.
		 *
		 * @see	_updateParents
		 */
		void setUpdateParent(GUIElementBase* updateParent);

		GUIWidget* mParentWidget;
		GUIPanel* mAnchorParent;
		GUIElementBase* mUpdateParent;

		GUIElementBase* mParentElement;
		Vector<GUIElementBase*> mChildren;	
		bool mIsDisabled;
		bool mIsDirty;

		GUIDimensions mDimensions;

		Vector2I mOffset;
		UINT32 mWidth, mHeight;

		Rect2I mClipRect;
		UINT32 mPanelDepthRange;
		UINT32 mDepth;
	};
}