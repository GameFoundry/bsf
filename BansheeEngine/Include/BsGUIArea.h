#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIArea
	{
	public:
		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			If you want the area to expand vertically or horizontally, together with its parent
		 *			widget, set height or width to 0, respectively.
		 */
		GUIArea(const HGUIWidget& widget, UINT32 x, UINT32 y, UINT32 width = 0, UINT32 height = 0);
		~GUIArea();

		GUILayout* getLayout() const { return mLayout; }

		UINT32 getDepth() const { return mDepth; }
		void setDepth(UINT32 depth) { mDepth = depth; }

	private:
		const HGUIWidget& mWidget;
		UINT32 mX, mY, mWidth, mHeight;
		UINT32 mDepth;

		GUILayout* mLayout;
	};
}