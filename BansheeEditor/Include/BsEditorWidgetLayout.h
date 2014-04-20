#pragma once

#include "BsEditorPrerequisites.h"
#include "CmIReflectable.h"

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

			Vector<String>::type widgetNames;
			bool isDocked;
			INT32 x, y;
			UINT32 width, height;
		};

	private:
		struct PrivatelyConstruct {};

	public:
		EditorWidgetLayout(const DockManagerLayoutPtr& dockLayout);
		EditorWidgetLayout(const PrivatelyConstruct& dummy) { }

		Vector<Entry>::type& getEntries() { return mEntries; }
		const DockManagerLayoutPtr& getDockLayout() const { return mDockLayout; }

	private:
		Vector<Entry>::type mEntries;
		DockManagerLayoutPtr mDockLayout;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/
	public:
		friend class EditorWidgetLayoutRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}