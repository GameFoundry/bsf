#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsVirtualInput.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	class GUITreeView : public GUIElementContainer
	{
	protected:
		enum class ScrollState
		{
			None,
			Up,
			Down,
			TransitioningUp,
			TransitioningDown
		};

		struct TreeElement
		{
			TreeElement();
			virtual ~TreeElement();

			TreeElement* mParent;
			Vector<TreeElement*>::type mChildren;

			GUIToggle* mFoldoutBtn;
			GUILabel* mElement;

			String mName;

			UINT32 mSortedIdx;
			bool mIsExpanded;
			bool mIsSelected;
			bool mIsVisible;

			bool isParentRec(TreeElement* element) const;
		};

		struct InteractableElement
		{
			InteractableElement(TreeElement* parent, UINT32 index, const RectI& bounds)
				:parent(parent), index(index), bounds(bounds)
			{ }

			bool isTreeElement() const { return index % 2 == 1; }
			TreeElement* getTreeElement() const;

			TreeElement* parent;
			UINT32 index;
			RectI bounds;
		};

		struct SelectedElement
		{
			SelectedElement(TreeElement* elem, GUITexture* back)
				:element(elem), background(back)
			{ }

			TreeElement* element;
			GUITexture* background;
		};

	public:
		static const String& getGUITypeName();

		void update();

	protected:
		virtual ~GUITreeView();

		Vector2I _getOptimalSize() const;
		void updateClippedBounds();

		virtual void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);
	protected:
		static const UINT32 ELEMENT_EXTRA_SPACING;
		static const UINT32 INDENT_SIZE;
		static const UINT32 INITIAL_INDENT_OFFSET;
		static const UINT32 DRAG_MIN_DISTANCE;
		static const float AUTO_EXPAND_DELAY_SEC;
		static const float SCROLL_AREA_HEIGHT_PCT;
		static const UINT32 SCROLL_SPEED_PX_PER_SEC;

		String mBackgroundStyle;
		String mElementBtnStyle;
		String mFoldoutBtnStyle;
		String mSelectionBackgroundStyle;
		String mEditBoxStyle;
		String mDragHighlightStyle;
		String mDragSepHighlightStyle;

		GUITexture* mBackgroundImage;

		Vector<InteractableElement>::type mVisibleElements;

		bool mIsElementSelected;
		Vector<SelectedElement>::type mSelectedElements;

		TreeElement* mEditElement;
		GUITreeViewEditBox* mNameEditBox;

		Vector2I mDragStartPosition;
		Vector2I mDragPosition;
		bool mDragInProgress;
		GUITexture* mDragHighlight;
		GUITexture* mDragSepHighlight;

		RectI mTopScrollBounds;
		RectI mBottomScrollBounds;
		ScrollState mScrollState;
		float mLastScrollTime;

		Stack<TreeElement*>::type mAutoExpandedElements;
		TreeElement* mMouseOverDragElement;
		float mMouseOverDragElementTime;

		static VirtualButton mRenameVB;
		static VirtualButton mDeleteVB;

		GUITreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, 
			const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUILayoutOptions& layoutOptions);

		const GUITreeView::InteractableElement* findElementUnderCoord(const Vector2I& coord) const;
		TreeElement* getTopMostSelectedElement() const;
		TreeElement* getBottomMostSelectedElement() const;

		void enableEdit(TreeElement* element);
		void disableEdit(bool acceptChanges);

		virtual bool mouseEvent(const GUIMouseEvent& ev);
		virtual bool commandEvent(const GUICommandEvent& ev);
		virtual bool virtualButtonEvent(const GUIVirtualButtonEvent& ev);
		void elementToggled(TreeElement* element, bool toggled);

		virtual TreeElement& getRootElement() = 0;
		virtual const TreeElement& getRootElementConst() const = 0;
		virtual void updateTreeElementHierarchy() = 0;
		virtual void renameTreeElement(TreeElement* element, const WString& name) = 0;
		virtual void deleteTreeElement(TreeElement* element) = 0;
		virtual bool acceptDragAndDrop() const = 0;
		virtual void dragAndDropStart() = 0;
		virtual void dragAndDropEnded(TreeElement* overTreeElement) = 0;
		virtual void dragAndDropFinalize() = 0;

		bool isSelectionActive() const;
		void selectElement(TreeElement* element);
		void unselectElement(TreeElement* element);
		void unselectAll();

		void expandElement(TreeElement* element);
		void collapseElement(TreeElement* element);

		void updateElementGUI(TreeElement* element);

		void closeTemporarilyExpandedElements();
		void temporarilyExpandElement(const GUITreeView::InteractableElement* mouseOverElement);
		void scrollToElement(TreeElement* element, bool center);

		GUIScrollArea* findParentScrollArea() const;

		void onEditAccepted();
		void onEditCanceled();
	};
}