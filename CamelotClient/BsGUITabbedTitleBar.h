#pragma once

#include "BsPrerequisites.h"
#include "BsGUIWidget.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class TabbedTitleBar : public BS::GUIWidget
	{
	public:
		TabbedTitleBar(const CM::HSceneObject& parent);
		virtual ~TabbedTitleBar();

		virtual void initialize(CM::Viewport* target, const CM::RenderWindow* ownerWindow);

		void addTab(const CM::String& name);
		void insertTab(CM::UINT32 idx, const CM::String& name);
		void removeTab(CM::UINT32 idx);

		boost::signal<void(CM::UINT32)> onTabActivated;
		boost::signal<void(CM::UINT32)> onTabAdded;
		boost::signal<void(CM::UINT32)> onTabRemoved;
	protected:
		CM::vector<BS::GUITexture*>::type mDragDropElements;
		CM::vector<BS::GUIToggle*>::type mTabButtons;

		BS::GUIArea* mMainArea;
		BS::GUIButton* mMinBtn;
		BS::GUIButton* mCloseBtn;
		BS::GUITexture* mLastDropElement;

		virtual void update();

		virtual bool _mouseEvent(BS::GUIElement* element, const BS::GUIMouseEvent& ev);
	};
}