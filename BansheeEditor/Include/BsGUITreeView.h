#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsVirtualInput.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUITreeView : public BS::GUIElementContainer
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
			CM::Vector<TreeElement*>::type mChildren;

			BS::GUIToggle* mFoldoutBtn;
			BS::GUILabel* mElement;

			CM::String mName;

			CM::UINT32 mSortedIdx;
			bool mIsExpanded;
			bool mIsSelected;
			bool mIsVisible;

			bool isParentRec(TreeElement* element) const;
		};

		struct InteractableElement
		{
			InteractableElement(TreeElement* parent, CM::UINT32 index, const CM::RectI& bounds)
				:parent(parent), index(index), bounds(bounds)
			{ }

			bool isTreeElement() const { return index % 2 == 1; }
			TreeElement* getTreeElement() const;

			TreeElement* parent;
			CM::UINT32 index;
			CM::RectI bounds;
		};

		struct SelectedElement
		{
			SelectedElement(TreeElement* elem, BS::GUITexture* back)
				:element(elem), background(back)
			{ }

			TreeElement* element;
			BS::GUITexture* background;
		};

	public:
		static const CM::String& getGUITypeName();

		void update();

	protected:
		virtual ~GUITreeView();

		CM::Vector2I _getOptimalSize() const;
		void updateClippedBounds();

		virtual void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
	protected:
		static const CM::UINT32 ELEMENT_EXTRA_SPACING;
		static const CM::UINT32 INDENT_SIZE;
		static const CM::UINT32 INITIAL_INDENT_OFFSET;
		static const CM::UINT32 DRAG_MIN_DISTANCE;
		static const float AUTO_EXPAND_DELAY_SEC;
		static const float SCROLL_AREA_HEIGHT_PCT;
		static const CM::UINT32 SCROLL_SPEED_PX_PER_SEC;

		const BS::GUIElementStyle* mBackgroundStyle;
		const BS::GUIElementStyle* mElementBtnStyle;
		const BS::GUIElementStyle* mFoldoutBtnStyle;
		const BS::GUIElementStyle* mSelectionBackgroundStyle;
		const BS::GUIElementStyle* mEditBoxStyle;
		const BS::GUIElementStyle* mDragHighlightStyle;
		const BS::GUIElementStyle* mDragSepHighlightStyle;

		BS::GUITexture* mBackgroundImage;

		CM::Vector<InteractableElement>::type mVisibleElements;

		bool mIsElementSelected;
		CM::Vector<SelectedElement>::type mSelectedElements;

		TreeElement* mEditElement;
		GUITreeViewEditBox* mNameEditBox;

		CM::Vector2I mDragStartPosition;
		CM::Vector2I mDragPosition;
		bool mDragInProgress;
		BS::GUITexture* mDragHighlight;
		BS::GUITexture* mDragSepHighlight;

		CM::RectI mTopScrollBounds;
		CM::RectI mBottomScrollBounds;
		ScrollState mScrollState;
		float mLastScrollTime;

		CM::Stack<TreeElement*>::type mAutoExpandedElements;
		TreeElement* mMouseOverDragElement;
		float mMouseOverDragElementTime;

		static BS::VirtualButton mRenameVB;
		static BS::VirtualButton mDeleteVB;

		GUITreeView(BS::GUIElementStyle* backgroundStyle, BS::GUIElementStyle* elementBtnStyle, 
			BS::GUIElementStyle* foldoutBtnStyle, BS::GUIElementStyle* selectionBackgroundStyle, BS::GUIElementStyle* editBoxStyle, 
			BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const BS::GUILayoutOptions& layoutOptions);

		const GUITreeView::InteractableElement* findElementUnderCoord(const CM::Vector2I& coord) const;
		TreeElement* getTopMostSelectedElement() const;
		TreeElement* getBottomMostSelectedElement() const;

		void enableEdit(TreeElement* element);
		void disableEdit(bool acceptChanges);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
		virtual bool commandEvent(const BS::GUICommandEvent& ev);
		virtual bool virtualButtonEvent(const BS::GUIVirtualButtonEvent& ev);
		void elementToggled(TreeElement* element, bool toggled);

		virtual TreeElement& getRootElement() = 0;
		virtual const TreeElement& getRootElementConst() const = 0;
		virtual void updateTreeElementHierarchy() = 0;
		virtual void renameTreeElement(TreeElement* element, const CM::WString& name) = 0;
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

		BS::GUIScrollArea* findParentScrollArea() const;

		void onEditAccepted();
		void onEditCanceled();
	};
}