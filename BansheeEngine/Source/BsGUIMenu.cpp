#include "BsGUIMenu.h"
#include "BsGUIDropDownBox.h"

namespace BansheeEngine
{
	GUIMenuItem::GUIMenuItem(GUIMenuItem* parent, const WString& name, std::function<void()> callback)
		:mParent(parent), mName(name), mCallback(callback), mIsSeparator(false)
	{

	}

	GUIMenuItem::GUIMenuItem(GUIMenuItem* parent)
		:mParent(parent), mCallback(nullptr), mIsSeparator(true)
	{

	}

	GUIMenuItem::~GUIMenuItem()
	{
		for(auto& child : mChildren)
			cm_delete<PoolAlloc>(child);
	}

	const GUIMenuItem* GUIMenuItem::findChild(const WString& name) const
	{
		auto iterFind = std::find_if(begin(mChildren), end(mChildren), [&] (GUIMenuItem* x) { return x->getName() == name; });

		if(iterFind != mChildren.end())
			return *iterFind;

		return nullptr;
	}

	GUIMenuItem* GUIMenuItem::findChild(const WString& name)
	{
		auto iterFind = std::find_if(begin(mChildren), end(mChildren), [&] (GUIMenuItem* x) { return x->getName() == name; });

		if(iterFind != mChildren.end())
			return *iterFind;

		return nullptr;
	}

	void GUIMenuItem::removeChild(const WString& name)
	{
		auto iterFind = std::find_if(begin(mChildren), end(mChildren), [&] (GUIMenuItem* x) { return x->getName() == name; });

		if(iterFind != mChildren.end())
		{
			cm_delete<PoolAlloc>(*iterFind);
			mChildren.erase(iterFind);
		}
	}

	GUIMenu::GUIMenu()
		:mRootElement(nullptr, L"", nullptr)
	{

	}

	GUIMenu::~GUIMenu()
	{

	}

	const GUIMenuItem* GUIMenu::addMenuItem(const WString& path, std::function<void()> callback)
	{
		return addMenuItemInternal(path, callback, false);
	}

	const GUIMenuItem* GUIMenu::addSeparator(const WString& path)
	{
		return addMenuItemInternal(path, nullptr, true);
	}

	const GUIMenuItem* GUIMenu::addMenuItemInternal(const WString& path, std::function<void()> callback, bool isSeparator)
	{
		Vector<WString>::type pathElements = StringUtil::split(path, L"/");

		GUIMenuItem* curSubMenu = &mRootElement;
		for(UINT32 i = 0; i < (UINT32)pathElements.size(); i++)
		{
			if(pathElements[i] == L"")
				continue;

			const WString& pathElem = *(pathElements.begin() + i);
			GUIMenuItem* existingItem = curSubMenu->findChild(pathElem);

			if(existingItem == nullptr)
			{
				bool isLastElem = i == (UINT32)(pathElements.size() - 1);

				if(isLastElem)
					existingItem = cm_new<GUIMenuItem, PoolAlloc>(curSubMenu, pathElem, callback);
				else
				{
					const WString& nextPathElem = *(pathElements.begin() + i);

					existingItem = cm_alloc<GUIMenuItem, PoolAlloc>();
					existingItem = new (existingItem) GUIMenuItem(curSubMenu, pathElem, nullptr);
				}

				curSubMenu->addChild(existingItem);
			}

			curSubMenu = existingItem;
		}

		if(isSeparator)
		{
			GUIMenuItem* separatorItem = cm_new<GUIMenuItem, PoolAlloc>(curSubMenu);
			curSubMenu->addChild(separatorItem);

			return separatorItem;
		}

		return curSubMenu;
	}

	const GUIMenuItem* GUIMenu::getMenuItem(const WString& path) const
	{
		Vector<WString>::type pathElements = StringUtil::split(path, L"/");

		const GUIMenuItem* curSubMenu = &mRootElement;
		for(UINT32 i = 0; i < (UINT32)pathElements.size(); i++)
		{
			const WString& pathElem = *(pathElements.begin() + i);
			const GUIMenuItem* existingItem = curSubMenu->findChild(pathElem);

			if(existingItem == nullptr || existingItem->isSeparator())
				return nullptr;

			curSubMenu = existingItem;
		}

		return curSubMenu;
	}

	void GUIMenu::removeMenuItem(const GUIMenuItem* item)
	{
		GUIMenuItem* parent = item->mParent;
		assert(parent != nullptr);

		parent->removeChild(item->getName());
	}

	GUIDropDownData GUIMenu::getDropDownData() const
	{
		return getDropDownDataInternal(mRootElement);
	}

	void GUIMenu::setLocalizedName(const WString& menuItemLabel, const HString& localizedName)
	{
		mLocalizedEntryNames[menuItemLabel] = localizedName;
	}

	GUIDropDownData GUIMenu::getDropDownDataInternal(const GUIMenuItem& menu) const
	{
		GUIDropDownData dropDownData;

		for(auto& menuItem : menu.mChildren)
		{
			if(menuItem->isSeparator())
			{
				dropDownData.entries.push_back(GUIDropDownDataEntry::separator());
			}
			else
			{
				if(menuItem->getNumChildren() == 0)
				{
					dropDownData.entries.push_back(GUIDropDownDataEntry::button(menuItem->getName(), menuItem->getCallback()));
				}
				else
				{
					dropDownData.entries.push_back(GUIDropDownDataEntry::subMenu(menuItem->getName(), getDropDownDataInternal(*menuItem)));
				}
			}
		}

		dropDownData.localizedNames = mLocalizedEntryNames;

		return dropDownData;
	}
}