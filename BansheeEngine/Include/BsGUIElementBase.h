#pragma once

#include "BsPrerequisites.h"
#include "BsGUIMaterialInfo.h"
#include "BsGUIDimensions.h"
#include "BsGUILayoutData.h"
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

	protected:
		/**
		 * @brief	Flags that signal the state of the GUI element.
		 */
		enum GUIElementFlags
		{
			GUIElem_Dirty = 0x01,
			GUIElem_Hidden = 0x02,
			GUIElem_Disabled = 0x04,
			GUIElem_HiddenLocal = 0x08,
			GUIElem_DisabledLocal = 0x10
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
		 * @brief	Resets element dimensions to their initial values dictated by the element's style.
		 */
		virtual void resetDimensions();

		/**
		 * @brief	Hides or shows this element and recursively applies the same state to all the child elements.
		 * 			This will not remove the element from the layout, the room for it will still be reserved but it just
		 * 			won't be visible.
		 */
		void setVisible(bool visible);

		/**
		 * @brief	Enables or disables this element and recursively applies the same state to all the child elements.
		 * 			This has the same effect as ::setVisible, but when disabled it will also remove the element from the 
		 * 			layout, essentially having the same effect is if you destroyed the element.
		 */
		void setEnabled(bool enabled);

		/**
		 * @brief	Returns non-clipped bounds of the GUI element. Relative to a parent GUI panel.
		 *
		 * @param	relativeTo	Parent panel of the provided element relative to which to return the
		 *						bounds. If null the bounds relative to the first parent panel are returned.
		 *						Behavior is undefined if provided panel is not a parent of the element.
		 *
		 * @note	This call can be potentially expensive if the GUI state is dirty.
		 */
		Rect2I getBounds(GUIPanel* relativeTo = nullptr);

		/**
		 * @brief	Sets the bounds of the GUI element. Relative to a parent GUI panel.
		 * 			Equivalent to calling setPosition, setWidth and setHeight.
		 */
		void setBounds(const Rect2I& bounds);

		/**
		 * @brief	Returns non-clipped bounds of the GUI element. Relative to a parent GUI widget.
		 *
		 * @note	This call can be potentially expensive if the GUI state is dirty.
		 */
		Rect2I getGlobalBounds();

		/**
		 * @brief	Returns non-clipped visible bounds of the GUI element (bounds exclude the margins). Relative to the parent GUI panel.
		 *
		 * @note	This call can be potentially expensive as the bounds need to be calculated based on current GUI state.
		 */
		virtual Rect2I getVisibleBounds();

		/************************************************************************/
		/* 							INTERNAL METHODS                      		*/
		/************************************************************************/

		/**
		 * @brief	Updates child elements positions, sizes, clip rectangles and depths so they
		 *			fit into the provided bounds, while respecting their layout options. 
		 *
		 * @param	data	Layout data containing the necessary bounds and restrictions
		 *					to use for calculating the child element layout data.
		 *
		 * @note	Internal method.
		 */
		virtual void _updateLayout(const GUILayoutData& data);

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
		virtual void _updateLayoutInternal(const GUILayoutData& data);

		/**
		 * @brief	Calculates positions & sizes of all elements in the layout. This method expects a pre-allocated array to store the data in.
		 *
		 * @brief	layoutArea		Parent layout area to position the child elements in.
		 * @param	elementAreas	Array to hold output areas. Must be the same size as the number of child elements.
		 * @param	numElements		Size of the element areas array.
		 * @param	sizeRanges		Ranges of possible sizes used for the child elements. Array must be same size as elements array.
		 * @param	mySizeRange		Size range of this element.
		 */
		virtual void _getElementAreas(const Rect2I& layoutArea, Rect2I* elementAreas, UINT32 numElements,
			const Vector<LayoutSizeRange>& sizeRanges, const LayoutSizeRange& mySizeRange) const;

		/**
		 * @brief	Updates layout data that determines GUI elements final position & depth 
		 *			in the GUI widget.
		 *
		 * @note	Internal method.
		 */
		virtual void _setLayoutData(const GUILayoutData& data) { mLayoutData = data; }

		/**
		 * @brief	Retrieves layout data that determines GUI elements final position & depth
		 *			in the GUI widget.
		 *
		 * @note	Internal method.
		 */
		const GUILayoutData& _getLayoutData() const { return mLayoutData; }

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
		 * @brief	Calculates element size range constrained by its layout options.
		 *
		 * @note	Internal method.
		 */
		virtual LayoutSizeRange _calculateLayoutSizeRange() const ;

		/**
		 * @brief	Returns element size range constrained by its layout options. This is
		 *			different from ::_calculateLayoutSizeRange because this method may return
		 *			cached size range.
		 *
		 * @note	Internal method.
		 */
		virtual LayoutSizeRange _getLayoutSizeRange() const;

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
		CGUIWidget* _getParentWidget() const { return mParentWidget; }

		/**
		 * @brief	Returns if element is visible or hidden.
		 *
		 * @note	Internal method.
		 */
		bool _isVisible() const { return (mFlags & GUIElem_Hidden) == 0; }

		/**
		 * @brief	Returns if element is enabled or disabled.
		 *
		 * @note	Internal method.
		 */
		bool _isEnabled() const { return (mFlags & GUIElem_Disabled) == 0; }

		/**
		 * Internal version of ::setVisible that doesn't modify local visibility, instead it is only meant to be called
		 * on child elements of the element whose visibility was modified.
		 *  
		 * @copydoc setVisible
		 */
		void _setVisible(bool visible);

		/**
		 * Internal version of ::setEnabled that doesn't modify local state, instead it is only meant to be called
		 * on child elements of the element whose state was modified.
		 *  
		 * @copydoc setVisible
		 */
		void _setEnabled(bool enabled);

		/**
		 * @brief	Changes the active GUI element widget. This allows you to move an element
		 *			to a different viewport, or change element style by using a widget with a different skin.
		 *			You are allowed to pass null here, but elements with no parent will be unmanaged. You will be
		 *			responsible for deleting them manually, and they will not render anywhere.
		 *
		 * @note	Internal method.
		 */
		virtual void _changeParentWidget(CGUIWidget* widget);

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
		 * @brief	Checks if element has been destroyed and is queued for deletion.
		 *
		 * @note	Internal method.
		 */
		virtual bool _isDestroyed() const { return false; }

		/**
		 * @brief	Marks the element's dimensions as dirty, triggering a layout rebuild.
		 *
		 * @note	Internal method.
		 */
		void _markLayoutAsDirty();

		/**
		 * @brief	Marks the element's contents as dirty, which causes the sprite meshes to be recreated from scratch.
		 *
		 * @note	Internal method.
		 */
		void _markContentAsDirty();

		/**
		 * @brief	Mark only the elements that operate directly on the sprite mesh without requiring the mesh
		 * 			to be recreated as dirty. This includes position, depth and clip rectangle. This will cause
		 *			the parent widget mesh to be rebuilt from its child element's meshes.
		 *
		 * @note	Internal method.
		 */
		void _markMeshAsDirty();

		/**
		 * @brief	Returns true if elements contents have changed since last update.
		 */
		bool _isDirty() const { return (mFlags & GUIElem_Dirty) != 0; }

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

		CGUIWidget* mParentWidget;
		GUIPanel* mAnchorParent;
		GUIElementBase* mUpdateParent;
		GUIElementBase* mParentElement;

		Vector<GUIElementBase*> mChildren;	
		UINT8 mFlags;

		GUIDimensions mDimensions;
		GUILayoutData mLayoutData;
	};
}