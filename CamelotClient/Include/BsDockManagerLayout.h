#pragma once

#include "BsEditorPrerequisites.h"
#include "CmIReflectable.h"

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

			static Entry* createLeaf(Entry* parent, CM::UINT32 childIdx, const CM::Vector<CM::String>::type& widgetNames);
			static Entry* createContainer(Entry* parent, CM::UINT32 childIdx, CM::UINT32 firstChildSize, bool horizontalSplit);

			CM::Vector<CM::String>::type widgetNames;
			bool isLeaf;
			CM::UINT32 firstChildSize;
			bool horizontalSplit;

			Entry* children[2];
			Entry* parent;
		};

	public:
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