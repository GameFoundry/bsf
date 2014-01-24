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

	public:
		static const CM::String& getGUITypeName();

		static GUISceneTreeView* create(BS::GUIWidget& parent,
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr, BS::GUIElementStyle* selectionBackgroundStyle = nullptr,
			BS::GUIElementStyle* editBoxStyle = nullptr);

		static GUISceneTreeView* create(BS::GUIWidget& parent, const BS::GUIOptions& options, 
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr, BS::GUIElementStyle* selectionBackgroundStyle = nullptr,
			BS::GUIElementStyle* editBoxStyle = nullptr);

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

		const BS::GUIElementStyle* mBackgroundStyle;
		const BS::GUIElementStyle* mElementBtnStyle;
		const BS::GUIElementStyle* mFoldoutBtnStyle;
		const BS::GUIElementStyle* mSelectionBackgroundStyle;
		const BS::GUIElementStyle* mEditBoxStyle;

		BS::GUITexture* mBackgroundImage;
		TreeElement mRootElement;

		CM::Vector<InteractableElement>::type mVisibleElements;
		CM::Vector<bool>::type mTempToDelete;

		TreeElement* mSelectedElement;
		BS::GUITexture* mSelectionBackground;
		
		TreeElement* mEditElement;
		GUITreeViewEditBox* mNameEditBox;

		GUISceneTreeView(BS::GUIWidget& parent, BS::GUIElementStyle* backgroundStyle, BS::GUIElementStyle* elementBtnStyle, 
			BS::GUIElementStyle* foldoutBtnStyle, BS::GUIElementStyle* selectionBackgroundStyle, BS::GUIElementStyle* editBoxStyle, 
			const BS::GUILayoutOptions& layoutOptions);

		const GUISceneTreeView::InteractableElement* findElementUnderCoord(const CM::Vector2I& coord) const;
		GUISceneTreeView::TreeElement* GUISceneTreeView::interactableToRealElement(const GUISceneTreeView::InteractableElement& element);

		void enableEdit(TreeElement* element);
		void disableEdit(bool acceptChanges);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
		virtual bool commandEvent(const BS::GUICommandEvent& ev);
		void elementToggled(TreeElement* element, bool toggled);

		void onEditAccepted();
		void onEditCanceled();
	};
}