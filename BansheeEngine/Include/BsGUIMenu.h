#pragma once

#include "BsPrerequisites.h"
#include "BsGUIWidget.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIMenuItem
	{
		struct Comparer 
		{
			bool operator() (const GUIMenuItem* lhs, GUIMenuItem* rhs) const
			{
				return lhs->mName.compare(rhs->mName) < 0;
			}
		};

	public:
		GUIMenuItem(GUIMenuItem* parent, const CM::WString& name, std::function<void()> callback);
		~GUIMenuItem();

		void addChild(GUIMenuItem* child) { mChildren.insert(child); }

		CM::UINT32 getNumChildren() const { return (CM::UINT32)mChildren.size(); }
		const GUIMenuItem* getParent() const { return mParent; }
		const CM::WString& getName() const { return mName; }
		const GUIMenuItem* findChild(const CM::WString& name) const;

		CM::Set<GUIMenuItem*, Comparer>::type::const_iterator childBegin() const { return mChildren.cbegin(); }
		CM::Set<GUIMenuItem*, Comparer>::type::const_iterator childEnd() const { return mChildren.cend(); }
		void removeChild(const CM::WString& name);

	private:
		friend class GUIMenu;

		GUIMenuItem* mParent;
		CM::WString mName;
		std::function<void()> mCallback;
		CM::Set<GUIMenuItem*, Comparer>::type mChildren;

		CM::Set<GUIMenuItem*, Comparer>::type::iterator findChildInternal(const CM::WString& name) const;
		
	};

	class BS_EXPORT GUIMenu : public GUIWidget
	{
	public:
		GUIMenu(const CM::HSceneObject& parent);
		~GUIMenu();

		void addMenuItem(const CM::WString& path, std::function<void()> callback);
		const GUIMenuItem* getMenuItem(const CM::WString& path) const;
		void removeMenuItem(const GUIMenuItem* item);
	protected:
		GUIMenuItem mRootElement;

		void openMenu(const GUIMenuItem* menuItem, const CM::WString& subMenuName);
	};
}