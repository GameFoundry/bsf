#pragma once

#include "BsPrerequisites.h"
#include "BsGUIDropDownBox.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIMenuItem
	{
	public:
		GUIMenuItem(GUIMenuItem* parent, const WString& name, std::function<void()> callback);
		GUIMenuItem(GUIMenuItem* parent);
		~GUIMenuItem();

		void addChild(GUIMenuItem* child) { mChildren.push_back(child); }

		UINT32 getNumChildren() const { return (UINT32)mChildren.size(); }
		const GUIMenuItem* getParent() const { return mParent; }
		const WString& getName() const { return mName; }
		std::function<void()> getCallback() const { return mCallback; }
		bool isSeparator() const { return mIsSeparator; }
		const GUIMenuItem* findChild(const WString& name) const;

		void removeChild(const WString& name);

	private:
		friend class GUIMenu;

		GUIMenuItem* mParent;
		bool mIsSeparator;
		WString mName;
		std::function<void()> mCallback;
		Vector<GUIMenuItem*>::type mChildren;

		GUIMenuItem* findChild(const WString& name);
	};

	class BS_EXPORT GUIMenu
	{
	public:
		GUIMenu();
		virtual ~GUIMenu();

		const GUIMenuItem* addMenuItem(const WString& path, std::function<void()> callback);
		const GUIMenuItem* addSeparator(const WString& path);
		const GUIMenuItem* getMenuItem(const WString& path) const;
		void removeMenuItem(const GUIMenuItem* item);

		GUIDropDownData getDropDownData() const;

		/**
		 * @brief	Normally menu items use values from their paths as their names. However path labels don't provide a way of
		 * 			localizing the menu item. This method allows you to set specific names (different from path labels)
		 * 			to each menu item. All the values are localized so they will also be updated according to the string table.
		 *
		 * @param	menuItemLabel	The menu item label. (e.g. if you have a menu like "View/Toolbars/Find, this parameter would be either
		 * 							"View", "Toolbars" or "Find" depending which entry you want to localize)
		 * @param	localizedName	Localized string with the name.
		 */
		void setLocalizedName(const WString& menuItemLabel, const HString& localizedName);
	protected:
		GUIMenuItem mRootElement;
		UnorderedMap<WString, HString>::type mLocalizedEntryNames;

		const GUIMenuItem* addMenuItemInternal(const WString& path, std::function<void()> callback, bool isSeparator);
		GUIDropDownData getDropDownDataInternal(const GUIMenuItem& menu) const;
	};
}