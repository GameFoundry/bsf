#pragma once

#include "BsEditorPrerequisites.h"
#include "CmIReflectable.h"
#include "CmRectI.h"

namespace BansheeEngine
{
	class DockManagerLayout : public IReflectable
	{
	public:
		struct Entry
		{
		public:
			Entry();
			~Entry();

			static Entry* createLeaf(Entry* parent, UINT32 childIdx, 
				const Vector<String>& widgetNames);
			static Entry* createContainer(Entry* parent, UINT32 childIdx, float splitPosition, 
				bool horizontalSplit);

			Vector<String> widgetNames;
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
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;	
	};
}