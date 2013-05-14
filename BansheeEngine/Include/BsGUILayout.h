#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUILayout
	{
		struct GUILayoutEntry
		{
			union
			{
				GUIElement* element;
				GUILayout* layout;
			};

			bool isLayout;
		};

	public:
		GUILayout();
		virtual ~GUILayout();

		void addElement(GUIElement* element);
		void removeElement(GUIElement* element);
		void insertElement(UINT32 idx, GUIElement* element);

		void addLayout(GUILayout* layout);
		void removeLayout(GUILayout* layout);
		void insertLayout(UINT32 idx, GUILayout* layout);

		/**
		 * @brief	Returns a combined number of child elements and layouts.
		 */
		UINT32 getNumChildren() const;

	private:
		std::vector<GUILayoutEntry> mChildren;
		GUILayout* mParentLayout;

		GUILayout* getParentLayout() const { return mParentLayout; }
		void setParentLayout(GUILayout* layout) { mParentLayout = layout; }
	};
}