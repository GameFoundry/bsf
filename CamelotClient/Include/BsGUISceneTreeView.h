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

			BS::GUIButton* mFoldoutBtn;
			BS::GUILabel* mElement;

			CM::HSceneObject mSceneObject;
			CM::String mName;
			CM::UINT32 mId;

			CM::UINT32 mSortedIdx;
			bool mIsExpanded;
			bool mIsDirty;
			bool mIsVisible;
		};

	public:
		static const CM::String& getGUITypeName();

		static GUISceneTreeView* create(BS::GUIWidget& parent,
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr);

		static GUISceneTreeView* create(BS::GUIWidget& parent, const BS::GUIOptions& options, 
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr);

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

		BS::GUITexture* mBackgroundImage;
		TreeElement mRootElement;

		CM::Vector<bool>::type mTempToDelete;

		GUISceneTreeView(BS::GUIWidget& parent, BS::GUIElementStyle* backgroundStyle, BS::GUIElementStyle* elementBtnStyle, 
			BS::GUIElementStyle* foldoutBtnStyle, const BS::GUILayoutOptions& layoutOptions);

		virtual bool mouseEvent(const BS::GUIMouseEvent& ev);
	};
}