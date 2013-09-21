#include "BsGUIMenu.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIMenuItem::GUIMenuItem(GUIMenuItem* parent, const CM::WString& name, std::function<void()> callback)
		:mParent(parent), mName(name), mCallback(callback)
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

	CM::Set<GUIMenuItem*, GUIMenuItem::Comparer>::type::iterator GUIMenuItem::findChildInternal(const CM::WString& name) const
	{
		GUIMenuItem tempItem(nullptr, name, nullptr);
		return mChildren.find(&tempItem);
	}

	GUIMenu::GUIMenu(const CM::HSceneObject& parent)
		:GUIWidget(parent), mRootElement(nullptr, L"", nullptr)
	{

	}

	GUIMenu::~GUIMenu()
	{

	}

	void GUIMenu::addMenuItem(const CM::WString& path, std::function<void()> callback)
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
					existingItem = new (existingItem) GUIMenuItem(curSubMenu, pathElem, boost::bind(&GUIMenu::openMenu, this, existingItem, nextPathElem));
				}

				curSubMenu->addChild(existingItem);
			}

			curSubMenu = existingItem;
		}
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

			if(existingItem == nullptr)
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

	void GUIMenu::openMenu(const GUIMenuItem* menuItem, const WString& subMenuName)
	{
		assert(menuItem != nullptr);

		const GUIMenuItem* subMenu = menuItem->findChild(subMenuName);
		assert(subMenu != nullptr);

		// TODO
	}
}