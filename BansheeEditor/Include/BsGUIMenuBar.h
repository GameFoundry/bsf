#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	class GUIMenuBar
	{
		struct GUIMenuBarData
		{
			WString name;
			GUIMenu* menu;
			GUIButton* button;
		};

	public:
		GUIMenuBar(GUIWidget* parent, RenderWindow* parentWindow);
		virtual ~GUIMenuBar();

		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);

		const GUIMenuItem* addMenuItem(const WString& path, std::function<void()> callback);
		const GUIMenuItem* addSeparator(const WString& path);
		const GUIMenuItem* getMenuItem(const WString& path) const;
		void removeMenuItem(const WString& path);
	private:
		static const UINT32 NUM_ELEMENTS_AFTER_CONTENT;

		RenderWindow* mParentWindow;
		GUIWidget* mParentWidget;
		GUIArea* mMainArea;
		GUIArea* mBackgroundArea;
		GUITexture* mBgTexture;
		GUITexture* mLogoTexture;

		GUIButton* mMinBtn;
		GUIButton* mMaxBtn;
		GUIButton* mCloseBtn;

		Vector<GUIMenuBarData>::type mChildMenus;

		GUIButton* mSubMenuButton;
		bool mSubMenuOpen;

		const GUIMenuBarData* getSubMenu(const WString& name) const;

		GUIMenuBarData* addNewButton(const WString& name);

		/**
		 * @brief	Attempts to remove the first element from the specified path. First element
		 * 			returned in specified in "pathRoot", and original "path" parameter is modified so
		 * 			it no longer includes the first element.
		 *
		 * @return	False if first element doesn't exist, true otherwise.
		 */
		bool stripPath(WString& path, WString& pathRoot) const;

		void openSubMenu(const WString& name);
		void closeSubMenu();

		void onSubMenuHover(const WString& name);
		void onSubMenuClosed();

		void onMinimizeClicked();
		void onMaximizeClicked();
		void onCloseClicked();

		void refreshNonClientAreas();
	};
}