#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	class EditorWidgetLayout : public IReflectable
	{
	public:
		struct Entry
		{
		public:
			Entry();
			~Entry();

			Vector<String> widgetNames;
			bool isDocked;
			INT32 x, y;
			UINT32 width, height;
		};

	private:
		struct PrivatelyConstruct {};

	public:
		EditorWidgetLayout(const DockManagerLayoutPtr& dockLayout);
		EditorWidgetLayout(const PrivatelyConstruct& dummy);

		Vector<Entry>& getEntries() { return mEntries; }
		const DockManagerLayoutPtr& getDockLayout() const { return mDockLayout; }

		void setIsMainWindowMaximized(bool maximized) { mMaximized = maximized; }
		bool getIsMainWindowMaximized() const { return mMaximized; }

	private:
		Vector<Entry> mEntries;
		DockManagerLayoutPtr mDockLayout;
		bool mMaximized;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class EditorWidgetLayoutRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}