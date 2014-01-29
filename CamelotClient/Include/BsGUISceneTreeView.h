#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUISceneTreeView : public BS::GUIElementContainer
	{
		struct TreeElement
		{
			TreeElement();
			~TreeElement();

			TreeElement* mParent;
			CM::Vector<TreeElement*>::type mChildren;

			BS::GUIToggle* mFoldoutBtn;
			BS::GUILabel* mElement;

			CM::HSceneObject mSceneObject;
			CM::String mName;
			CM::UINT32 mId;

			CM::UINT32 mSortedIdx;
			bool mIsExpanded;
			bool mIsSelected;
			bool mIsDirty;
			bool mIsVisible;
		};

		struct InteractableElement
		{
			InteractableElement(TreeElement* parent, CM::UINT32 index, const CM::RectI& bounds)
				:parent(parent), index(index), bounds(bounds)
			{ }

			bool isTreeElement() const { return index % 2 == 1; }

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

		struct DraggedSceneObjects
		{
			DraggedSceneObjects(CM::UINT32 numObjects);
			~DraggedSceneObjects();

			CM::UINT32 numObjects;
			CM::HSceneObject* objects;
		};

	public:
		static const CM::String& getGUITypeName();

		static GUISceneTreeView* create(BS::GUIWidget& parent,
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr, BS::GUIElementStyle* selectionBackgroundStyle = nullptr,
			BS::GUIElementStyle* editBoxStyle = nullptr, BS::GUIElementStyle* dragHighlightStyle = nullptr, 
			BS::GUIElementStyle* dragSepHighlightStyle = nullptr);

		static GUISceneTreeView* create(BS::GUIWidget& parent, const BS::GUIOptions& options, 
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr, BS::GUIElementStyle* selectionBackgroundStyle = nullptr,
			BS::GUIElementStyle* editBoxStyle = nullptr, BS::GUIElementStyle* dragHighlightStyle = nullptr, 
			BS::GUIElementStyle* dragSepHighlightStyle = nullptr);

		void update();

	protected:
		virtual ~GUISceneTreeView();

		CM::Vector2I _getOptimalSize() const;
		void updateClippedBounds();

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);
	protected:
		static const CM::UINT32 ELEMENT_EXTRA_SPACING;
		static const CM::UINT32 INDENT_SIZE;
		static const CM::UINT32 INITIAL_INDENT_OFFSET;
		static const CM::UINT32 DRAG_MIN_DISTANCE;

		const BS::GUIElementStyle* mBackgroundStyle;
		const BS::GUIElementStyle* mElementBtnStyle;
		const BS::GUIElementStyle* mFoldoutBtnStyle;
		const BS::GUIElementStyle* mSelectionBackgroundStyle;
		const BS::GUIElementStyle* mEditBoxStyle;
		const BS::GUIElementStyle* mDragHighlightStyle;
		const BS::GUIElementStyle* mDragSepHighlightStyle;

		BS::GUITexture* mBackgroundImage;
		TreeElement mRootElement;

		CM::Vector<InteractableElement>::type mVisibleElements;
		CM::Vector<bool>::type mTempToDelete;

		bool mIsElementSelected;
		CM::Vector<SelectedElement>::type mSelectedElements;
		
		TreeElement* mEditElement;
		GUITreeViewEditBox* mNameEditBox;

		CM::Vector2I mDragStartPosition;
		CM::Vector2I mDragPosition;
		bool mDragInProgress;
		BS::GUITexture* mDragHighlight;
		BS::GUITexture* mDragSepHighlight;

		GUISceneTreeView(BS::GUIWidget& parent, BS::GUIElementStyle* backgroundStyle, BS::GUIElementStyle* elementBtnStyle, 
			BS::GUIElementStyle* foldoutBtnStyle, BS::GUIElementStyle* selectionBackgroundStyle, BS::GUIElementStyle* editBoxStyle, 
			BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const BS::GUILayoutOptions& layoutOptions);

		const GUISceneTreeView::InteractableElement* findElementUnderCoord(const CM::Vector2I& coord) const;
		GUISceneTreeView::TreeElement* GUISceneTreeView::interactableToRealElement(const GUISceneTreeView::InteractableElement& element);

		void enableEdit(TreeElement* element);
		void disableEdit(bool acceptChanges);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
		virtual bool commandEvent(const BS::GUICommandEvent& ev);
		void elementToggled(TreeElement* element, bool toggled);

		bool isSelectionActive() const;
		void selectElement(TreeElement* element);
		void unselectElement(TreeElement* element);
		void unselectAll();

		void onEditAccepted();
		void onEditCanceled();

		void dragAndDropEnded();
	};
}