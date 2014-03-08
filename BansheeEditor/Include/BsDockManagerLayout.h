#pragma once

#include "BsEditorPrerequisites.h"
#include "CmIReflectable.h"
#include "CmRectI.h"

namespace BansheeEditor
{
	class DockManagerLayout : public CM::IReflectable
	{
	public:
		struct Entry
		{
		public:
			Entry();
			~Entry();

			static Entry* createLeaf(Entry* parent, CM::UINT32 childIdx, 
				const CM::Vector<CM::String>::type& widgetNames);
			static Entry* createContainer(Entry* parent, CM::UINT32 childIdx, float splitPosition, 
				bool horizontalSplit);

			CM::Vector<CM::String>::type widgetNames;
			bool isLeaf;
			float splitPosition;
			bool horizontalSplit;

			Entry* children[2];
			Entry* parent;
		};

	public:
		~DockManagerLayout();

		Entry& getRootEntry() { return mRootEntry; }

	private:
		Entry mRootEntry;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class DockManagerLayoutRTTI;
		static CM::RTTITypeBase* getRTTIStatic();
		virtual CM::RTTITypeBase* getRTTI() const;	
	};
}