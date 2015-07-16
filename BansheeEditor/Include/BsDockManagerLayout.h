#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Used for storing a layout of widgets in a dock manager. 
	 *
	 * @see		DockManager
	 */
	class DockManagerLayout : public IReflectable
	{
	public:
		/**
		 * @brief	A single entry in the dock layout which may contain
		 *			references to two other entries (non-leaf) or may contain
		 *			multiple widgets (leaf).
		 */
		struct Entry
		{
		public:
			Entry();
			~Entry();

			/**
			 * @brief	Creates a new leaf entry with the specified widgets.
			 *
			 * @param	parent		Parent of this leaf entry. Can be null for root.
			 * @param	childIdx	Index of this entry in the parents child list. Can be 0 or 1.
			 * @param	widgetNames	A list of all widgets opened in this entry, listed by name.
			 */
			static Entry* createLeaf(Entry* parent, UINT32 childIdx, 
				const Vector<String>& widgetNames);

			/**
			 * @brief	Creates a new container entry with the specified split data.
			 *
			 * @param	parent				Parent of this leaf entry. Can be null for root.
			 * @param	childIdx			Index of this entry in the parents child list. Can be 0 or 1.
			 * @param	splitPosition		Determines at what position(in percent) should this container be split. 
			 * @param	horizontalSplit		Whether the split is horizontal (true) or vertical (false).
			 */
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

		/**
		 * @brief	Returns the root entry in the saved dock manager hierarchy.
		 */
		Entry& getRootEntry() { return mRootEntry; }

		/**
		 * @brief	Removes widgets that can no longer be found (their names no longer reference a widget),
		 *			and removes containers with no widgets.
		 */
		void pruneInvalidLeaves();

	private:
		Entry mRootEntry;

		/************************************************************************/
		/* 								RTTI		                     		*/
		/************************************************************************/

	public:
		friend class DockManagerLayoutRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const override;	
	};
}