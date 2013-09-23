#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIMenuBar
	{
		struct GUIMenuBarData
		{
			CM::WString name;
			GUIMenu* menu;
			GUIButton* button;
		};

	public:
		GUIMenuBar(BS::GUIWidget* parent);
		virtual ~GUIMenuBar();

		void setSize(CM::UINT32 width, CM::UINT32 height);
		void setPosition(CM::INT32 x, CM::INT32 y);

		const GUIMenuItem* addMenuItem(const CM::WString& path, std::function<void()> callback);
		const GUIMenuItem* addSeparator(const CM::WString& path);
		const GUIMenuItem* getMenuItem(const CM::WString& path) const;
		void removeMenuItem(const CM::WString& path);
	private:
		GUIWidget* mParentWidget;
		GUIArea* mMainArea;
		GUIArea* mBackgroundArea;

		CM::Vector<GUIMenuBarData>::type mChildMenus;

		const GUIMenuBarData* getSubMenu(const CM::WString& name) const;

		/**
		 * @brief	Attempts to remove the first element from the specified path. First element
		 * 			returned in specified in "pathRoot", and original "path" parameter is modified so
		 * 			it no longer includes the first element.
		 *
		 * @return	False if first element doesn't exist, true otherwise.
		 */
		bool stripPath(CM::WString& path, CM::WString& pathRoot) const;
	};
}