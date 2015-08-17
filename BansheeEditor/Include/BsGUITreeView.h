#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsVirtualInput.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	GUI element that displays some contents in a tree-view where elements
	 *			are placed vertically above/beneath each other, and different elements
	 *			may be nested within other elements.
	 *
	 *			Elements may be selected, renamed, dragged and re-parented.
	 *
	 *			This class is abstract and meant to be extended by an implementation 
	 *			specific to some content type (e.g. scene object hierarchy). 
	 */
	class BS_ED_EXPORT GUITreeView : public GUIElementContainer
	{
	protected:
		/**
		 * @brief	Types of possible states when the tree view is auto-scrolling.
		 */
		enum class ScrollState
		{
			None,
			Up,
			Down,
			TransitioningUp,
			TransitioningDown
		};

		/**
		 * @brief	Contains data about a single piece of content
		 *			and all its children. This element may be visible and represented
		 *			by a GUI element, but might not (e.g. its parent is collapsed).
		 */
		struct TreeElement
		{
			TreeElement();
			virtual ~TreeElement();

			TreeElement* mParent;
			Vector<TreeElement*> mChildren;

			GUIToggle* mFoldoutBtn;
			GUILabel* mElement;

			String mName;

			UINT32 mSortedIdx;
			bool mIsExpanded;
			bool mIsSelected;
			bool mIsHighlighted;
			bool mIsVisible;
			bool mIsGrayedOut;

			bool isParentRec(TreeElement* element) const;
		};

		/**
		 * @brief	Contains data about all visible elements in the tree view.
		 *			This might be a TreeElement entry, or a separator (empty space)
		 *			between two TreeElement%s.
		 */
		struct InteractableElement
		{
			InteractableElement(TreeElement* parent, UINT32 index, const Rect2I& bounds)
				:parent(parent), index(index), bounds(bounds)
			{ }

			bool isTreeElement() const { return index % 2 == 1; }
			TreeElement* getTreeElement() const;

			TreeElement* parent;
			UINT32 index;
			Rect2I bounds;
		};

		/**
		 * @brief	Contains data about one of the currently selected tree elements.
		 */
		struct SelectedElement
		{
			SelectedElement()
				:element(nullptr), background(nullptr)
			{ }

			SelectedElement(TreeElement* elem, GUITexture* back)
				:element(elem), background(back)
			{ }

			TreeElement* element;
			GUITexture* background;
		};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Updates tree view if dirty, among other operations.
		 *			Must be called every frame. 
		 *
		 * @note	Internal method.
		 */
		void _update();

	protected:
		GUITreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& highlightBackgroundStyle, const String& selectionBackgroundStyle, const String& editBoxStyle,
			const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUIDimensions& dimensions);

		virtual ~GUITreeView();

		/**
		 * @copydoc	GUIElement::_getOptimalSize
		 */
		Vector2I _getOptimalSize() const override;

		/**
		 * @copydoc	GUIElement::updateClippedBounds
		 */
		void updateClippedBounds() override;

		/**
		 * @copydoc	GUIElement::_updateLayoutInternal
		 */
		virtual void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUIElement::_mouseEvent
		 */
		virtual bool _mouseEvent(const GUIMouseEvent& ev) override;

		/**
		 * @copydoc	GUIElement::_commandEvent
		 */
		virtual bool _commandEvent(const GUICommandEvent& ev) override;

		/**
		 * @copydoc	GUIElement::_virtualButtonEvent
		 */
		virtual bool _virtualButtonEvent(const GUIVirtualButtonEvent& ev) override;

		/**
		 * @brief	Attempts to find an interactable element under the specified coordinates.
		 *			Returns null if one cannot be found.
		 *
		 * @param	coord	Coordinates relative to parent GUI widget.
		 */
		const GUITreeView::InteractableElement* findElementUnderCoord(const Vector2I& coord) const;

		/**
		 * @brief	Returns the top-most selected tree element if selection is active,
		 *			null otherwise.
		 */
		TreeElement* getTopMostSelectedElement() const;

		/**
		 * @brief	Returns the bottom-most selected tree element if selection is active,
		 *			null otherwise.
		 */
		TreeElement* getBottomMostSelectedElement() const;

		/**
		 * @brief	Starts rename operation on the specified tree element, spawning an input box.
		 */
		void enableEdit(TreeElement* element);

		/**
		 * @brief	Ends rename operation if one is currently active.
		 *
		 * @param	acceptChanges	Should the new name be accepted or discarded.
		 */
		void disableEdit(bool acceptChanges);

		/**
		 * @brief	Triggered when the Foldout button for a tree element was been toggled.
		 *			(i.e. something was expanded or collapsed).
		 */
		void elementToggled(TreeElement* element, bool toggled);

		/**
		 * @brief	Returns the top level TreeElement.
		 */
		virtual TreeElement& getRootElement() = 0;

		/**
		 * @brief	Returns the top level TreeElement that cannot be modified.
		 */
		virtual const TreeElement& getRootElementConst() const = 0;

		/**
		 * @brief	Checks if the hierarchy needs any updates and performs those
		 *			updates if needed.
		 */
		virtual void updateTreeElementHierarchy() = 0;

		/**
		 * @brief	Changes the name of the content associated with the provided tree element.
		 */
		virtual void renameTreeElement(TreeElement* element, const WString& name) = 0;

		/**
		 * @brief	Deletes the content associated with the provided tree element.
		 */
		virtual void deleteTreeElement(TreeElement* element) = 0;

		/**
		 * @brief	Checks whether the tree view can accept the currently active drag and drop
		 *			operation.
		 */
		virtual bool acceptDragAndDrop() const = 0;

		/**
		 * @brief	Triggered when the user drags a tree element and starts a drag and drop operation.
		 */
		virtual void dragAndDropStart() = 0;

		/**
		 * @brief	Triggered when the user ends a drag and drop operation over the tree view.
		 *
		 * @param	overTreeElement	TreeElement the drag operation ended over, if any.
		 */
		virtual void dragAndDropEnded(TreeElement* overTreeElement) = 0;

		/**
		 * @brief	Triggered whenever a TreeElement gets selected or deselected.
		 */
		virtual void selectionChanged() { }

		/**
		 * @brief	Are any tree elements currently selected.
		 */
		bool isSelectionActive() const;

		/**
		 * @brief	Expands the selection to the provided TreeElement. Doesn't clear
		 *			previous selection.
		 */
		void selectElement(TreeElement* element);

		/**
		 * @brief	Unselects the provided TreeElement.
		 */
		void unselectElement(TreeElement* element);

		/**
		 * @brief	Unselects all selected TreeElement%s.
		 */
		void unselectAll();

		/**
		 * @brief	Starts rename operation on the currently selected element.
		 */
		void renameSelected();

		/**
		 * @brief	Deletes all currently selected elements.
		 */
		void deleteSelection();

		/**
		 * @brief	Expands all parents of the provided TreeElement making it interactable.
		 */
		void expandToElement(TreeElement* element);

		/**
		 * @brief	Expands the provided TreeElement making its children interactable.
		 */
		void expandElement(TreeElement* element);

		/**
		 * @brief	Collapses the provided TreeElement making its children hidden and not interactable.
		 */
		void collapseElement(TreeElement* element);

		/**
		 * @brief	Duplicates the currently selected entries.
		 */
		virtual void duplicateSelection() { }

		/**
		 * @brief	Marks the current selection for copying.
		 */
		virtual void copySelection() { }
		
		/**
		 * @brief	Marks the current selection for cutting.
		 */
		virtual void cutSelection() { }

		/**
		 * @brief	Pastes a set of entries previously marked for cut or copy.
		 */
		virtual void paste() { }

		/**
		 * @brief	Rebuilds the needed GUI elements for the provided TreeElement.
		 */
		void updateElementGUI(TreeElement* element);

		/**
		 * @brief	Close any elements that were temporarily expanded due to a drag operation
		 *			hovering over them.
		 */
		void closeTemporarilyExpandedElements();

		/**
		 * @brief	Temporarily expand the provided element. Temporarily expanded elements can be
		 *			closed by calling ::closeTemporarilyExpandedElements.
		 */
		void temporarilyExpandElement(const GUITreeView::InteractableElement* mouseOverElement);

		/**
		 * @brief	Scrolls the parent GUIScrollArea (if any) so that the specified TreeElement is visible.
		 *
		 * @param	element	Element to scroll to.
		 * @param	center	If true the element will be centered in the scroll view,
		 *					otherwise it will be at the top. 
		 */
		void scrollToElement(TreeElement* element, bool center);

		/**
		 * @brief	Attempts to find the parent GUIScrollArea that the tree view is a child of.
		 */
		GUIScrollArea* findParentScrollArea() const;

		/**
		 * @brief	Scrolls the tree view to the specified element and highlights it.
		 */
		void ping(TreeElement* element);

		/**
		 * @brief	Clears the ping highlight on the currently highlighted element.
		 */
		void clearPing();

		/**
		 * @brief	Triggered when the user accepts the changes during a rename operation.
		 */
		void onEditAccepted();

		/**
		 * @brief	Triggered when the user rejects the changes during a rename operation.
		 */
		void onEditCanceled();

		/**
		 * @brief	Triggered when the user clicks outside of the edit box during a rename operation.
		 */
		void onEditFocusLost();

		String mBackgroundStyle;
		String mElementBtnStyle;
		String mFoldoutBtnStyle;
		String mSelectionBackgroundStyle;
		String mHighlightBackgroundStyle;
		String mEditBoxStyle;
		String mDragHighlightStyle;
		String mDragSepHighlightStyle;

		GUITexture* mBackgroundImage;

		Vector<InteractableElement> mVisibleElements;

		bool mIsElementSelected;
		Vector<SelectedElement> mSelectedElements;

		bool mIsElementHighlighted;
		SelectedElement mHighlightedElement;

		TreeElement* mEditElement;
		GUITreeViewEditBox* mNameEditBox;

		Vector2I mDragStartPosition;
		Vector2I mDragPosition;
		bool mDragInProgress;
		GUITexture* mDragHighlight;
		GUITexture* mDragSepHighlight;

		Rect2I mTopScrollBounds;
		Rect2I mBottomScrollBounds;
		ScrollState mScrollState;
		float mLastScrollTime;

		Stack<TreeElement*> mAutoExpandedElements;
		TreeElement* mMouseOverDragElement;
		float mMouseOverDragElementTime;

		static VirtualButton mRenameVB;
		static VirtualButton mDeleteVB;
		static VirtualButton mDuplicateVB;
		static VirtualButton mCutVB;
		static VirtualButton mCopyVB;
		static VirtualButton mPasteVB;

		static const UINT32 ELEMENT_EXTRA_SPACING;
		static const UINT32 INDENT_SIZE;
		static const UINT32 INITIAL_INDENT_OFFSET;
		static const UINT32 DRAG_MIN_DISTANCE;
		static const float AUTO_EXPAND_DELAY_SEC;
		static const float SCROLL_AREA_HEIGHT_PCT;
		static const UINT32 SCROLL_SPEED_PX_PER_SEC;
		static const Color GRAYED_OUT_COLOR;
	};
}