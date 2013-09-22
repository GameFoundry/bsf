#include "BsGUIMenu.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIMenuItem::GUIMenuItem(GUIMenuItem* parent, const CM::WString& name, std::function<void()> callback)
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

	const GUIMenuItem* GUIMenuItem::findChild(const CM::WString& name) const
	{
		auto iterFind = findChildInternal(name);

		if(iterFind != mChildren.end())
			return *iterFind;

		return nullptr;
	}

	void GUIMenuItem::removeChild(const CM::WString& name)
	{
		auto iterFind = findChildInternal(name);

		if(iterFind != mChildren.end())
		{
			cm_delete<PoolAlloc>(*iterFind);
			mChildren.erase(iterFind);
		}
	}

	CM::Vector<GUIMenuItem*>::type::const_iterator GUIMenuItem::findChildInternal(const CM::WString& name) const
	{
		return std::find_if(begin(mChildren), end(mChildren), [&] (GUIMenuItem* x) { return x->getName() == name; });
	}

	CM::Vector<GUIMenuItem*>::type::iterator GUIMenuItem::findChildInternal(const CM::WString& name)
	{
		return std::find_if(begin(mChildren), end(mChildren), [&] (GUIMenuItem* x) { return x->getName() == name; });
	}

	GUIMenu::GUIMenu()
		:mRootElement(nullptr, L"", nullptr)
	{

	}

	GUIMenu::~GUIMenu()
	{

	}

	const GUIMenuItem* GUIMenu::addMenuItem(const CM::WString& path, std::function<void()> callback)
	{
		return addMenuItemInternal(path, callback, false);
	}

	const GUIMenuItem* GUIMenu::addSeparator(const CM::WString& path)
	{
		return addMenuItemInternal(path, nullptr, true);
	}

	const GUIMenuItem* GUIMenu::addMenuItemInternal(const CM::WString& path, std::function<void()> callback, bool isSeparator)
	{
		Vector<WString>::type pathElements = StringUtil::split(path, L"/");

		GUIMenuItem* curSubMenu = &mRootElement;
		for(UINT32 i = 0; i < (UINT32)pathElements.size(); i++)
		{
			const WString& pathElem = *(pathElements.begin() + i);
			auto foundChild = curSubMenu->findChildInternal(pathElem);
			GUIMenuItem* existingItem = *foundChild;

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

	const GUIMenuItem* GUIMenu::getMenuItem(const CM::WString& path) const
	{
		Vector<WString>::type pathElements = StringUtil::split(path, L"/");

		const GUIMenuItem* curSubMenu = &mRootElement;
		for(UINT32 i = 0; i < (UINT32)pathElements.size(); i++)
		{
			const WString& pathElem = *(pathElements.begin() + i);
			auto foundChild = curSubMenu->findChildInternal(pathElem);
			GUIMenuItem* existingItem = *foundChild;

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
}