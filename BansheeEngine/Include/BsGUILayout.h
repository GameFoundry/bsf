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

		GUILayout& addLayoutX();
		GUILayout& addLayoutY();
		void removeLayout(GUILayout& layout);
		GUILayout& insertLayoutX(UINT32 idx);
		GUILayout& insertLayoutY(UINT32 idx);

		/**
		 * @brief	Returns a combined number of child elements and layouts.
		 */
		UINT32 getNumChildren() const;

	protected:
		virtual void update(UINT32 x, UINT32 y, UINT32 width, UINT32 height, UINT32 depth) = 0;

	private:
		friend class GUIArea;

		std::vector<GUILayoutEntry> mChildren;
	};
}