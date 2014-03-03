#pragma once

#include "BsEditorPrerequisites.h"
#include "CmIReflectable.h"

namespace BansheeEditor
{
	class EditorWidgetLayout : public CM::IReflectable
	{
	public:
		struct Entry
		{
		public:
			Entry();
			~Entry();

			CM::Vector<CM::String>::type widgetNames;
			bool isDocked;
			CM::INT32 x, y;
			CM::UINT32 width, height;
		};

	private:
		struct PrivatelyConstruct {};

	public:
		EditorWidgetLayout(const DockManagerLayoutPtr& dockLayout);
		EditorWidgetLayout(const PrivatelyConstruct& dummy) { }

		CM::Vector<Entry>::type& getEntries() { return mEntries; }
		const DockManagerLayoutPtr& getDockLayout() const { return mDockLayout; }

	private:
		CM::Vector<Entry>::type mEntries;
		DockManagerLayoutPtr mDockLayout;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class EditorWidgetLayoutRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;	
	};
}