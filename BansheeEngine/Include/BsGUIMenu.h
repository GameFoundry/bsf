#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIMenuItem
	{
	public:
		GUIMenuItem(GUIMenuItem* parent, const CM::WString& name, std::function<void()> callback);
		GUIMenuItem(GUIMenuItem* parent);
		~GUIMenuItem();

		void addChild(GUIMenuItem* child) { mChildren.push_back(child); }

		CM::UINT32 getNumChildren() const { return (CM::UINT32)mChildren.size(); }
		const GUIMenuItem* getParent() const { return mParent; }
		const CM::WString& getName() const { return mName; }
		std::function<void()> getCallback() const { return mCallback; }
		bool isSeparator() const { return mIsSeparator; }
		const GUIMenuItem* findChild(const CM::WString& name) const;

		void removeChild(const CM::WString& name);

	private:
		friend class GUIMenu;

		GUIMenuItem* mParent;
		bool mIsSeparator;
		CM::WString mName;
		std::function<void()> mCallback;
		CM::Vector<GUIMenuItem*>::type mChildren;

		GUIMenuItem* findChild(const CM::WString& name);
	};

	class BS_EXPORT GUIMenu
	{
	public:
		GUIMenu();
		virtual ~GUIMenu();

		const GUIMenuItem* addMenuItem(const CM::WString& path, std::function<void()> callback);
		const GUIMenuItem* addSeparator(const CM::WString& path);
		const GUIMenuItem* getMenuItem(const CM::WString& path) const;
		void removeMenuItem(const GUIMenuItem* item);

		CM::Vector<GUIDropDownData>::type getDropDownData() const;
	protected:
		GUIMenuItem mRootElement;

		const GUIMenuItem* addMenuItemInternal(const CM::WString& path, std::function<void()> callback, bool isSeparator);
		CM::Vector<GUIDropDownData>::type getDropDownDataInternal(const GUIMenuItem& menu) const;
	};
}