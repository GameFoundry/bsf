#pragma once

#include "BsEditorPrerequisites.h"
#include "BsIReflectable.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains stored positions and sizes of all editor 
	 *			widgets, whether docked or floating.
	 */
	class EditorWidgetLayout : public IReflectable
	{
	public:
		/**
		 * @brief	An entry representing widgets and area of a single
		 *			widget container.
		 */
		struct Entry
		{
		public:
			Entry();
			~Entry();

			Vector<String> widgetNames; /**< Unique names of all widgets in the container. */
			bool isDocked; /**< Whether the container is floating in its own window or docked. */
			INT32 x, y; /**< Position of the container relative to parent window or dock manager. */
			UINT32 width, height; /**< Size of the container. */
		};

	private:
		struct PrivatelyConstruct {};

	public:
		EditorWidgetLayout(const DockManagerLayoutPtr& dockLayout);
		EditorWidgetLayout(const PrivatelyConstruct& dummy);

		/**
		 * @brief	Returns saved representations of all widget containers. Each entry contains information
		 *			about a single widget container.
		 */
		Vector<Entry>& getEntries() { return mEntries; }

		/**
		 * @brief	Returns dock layout that contains data about how were widget containers docked in the
		 *			dock manager.
		 */
		const DockManagerLayoutPtr& getDockLayout() const { return mDockLayout; }

		/**
		 * @brief	Sets whether the main editor window is maximized.
		 */
		void setIsMainWindowMaximized(bool maximized) { mMaximized = maximized; }

		/**
		 * @brief	Checks whether the main editor window was maximized.
		 */
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
		virtual RTTITypeBase* getRTTI() const override;	
	};
}