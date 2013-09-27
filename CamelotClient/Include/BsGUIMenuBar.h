#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	class GUIMenuBar
	{
		struct GUIMenuBarData
		{
			CM::WString name;
			BS::GUIMenu* menu;
			BS::GUIButton* button;
		};

	public:
		GUIMenuBar(BS::GUIWidget* parent);
		virtual ~GUIMenuBar();

		void setArea(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);

		const BS::GUIMenuItem* addMenuItem(const CM::WString& path, std::function<void()> callback);
		const BS::GUIMenuItem* addSeparator(const CM::WString& path);
		const BS::GUIMenuItem* getMenuItem(const CM::WString& path) const;
		void removeMenuItem(const CM::WString& path);
	private:
		BS::GUIWidget* mParentWidget;
		BS::GUIArea* mMainArea;
		BS::GUIArea* mBackgroundArea;
		BS::GUITexture* mBgTexture;
		BS::GUITexture* mLogoTexture;

		CM::Vector<GUIMenuBarData>::type mChildMenus;

		bool mSubMenuOpen;

		const GUIMenuBarData* getSubMenu(const CM::WString& name) const;

		/**
		 * @brief	Attempts to remove the first element from the specified path. First element
		 * 			returned in specified in "pathRoot", and original "path" parameter is modified so
		 * 			it no longer includes the first element.
		 *
		 * @return	False if first element doesn't exist, true otherwise.
		 */
		bool stripPath(CM::WString& path, CM::WString& pathRoot) const;

		void openSubMenu(const CM::WString& name);
		void closeSubMenu();

		void onSubMenuClosed();
	};
}