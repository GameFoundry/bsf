#pragma once

#include "BsPrerequisites.h"

namespace BansheeEngine
{
	class BS_EXPORT GUIArea
	{
	public:
		~GUIArea();

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			If you want the area to expand vertically or horizontally, together with its parent
		 *			widget, set height or width to 0, respectively.
		 */
		static GUIArea* create(GUIWidget& widget, CM::UINT32 x, CM::UINT32 y, CM::UINT32 width = 0, CM::UINT32 height = 0, CM::UINT16 depth = 0);
		static void destroy(GUIArea* area);

		GUILayout& getLayout() const { return *mLayout; }

		CM::UINT16 getDepth() const { return mDepth; }
		void setDepth(CM::UINT16 depth) { mDepth = depth; }

		CM::UINT32 x() const { return mX; }
		CM::UINT32 y() const { return mY; }
		CM::UINT32 width() const { return mWidth; }
		CM::UINT32 height() const { return mHeight; }

		void _update();
	private:
		friend class GUIWidget;

		GUIWidget& mWidget;
		CM::UINT32 mX, mY, mWidth, mHeight;
		CM::UINT16 mDepth;
		bool mResizeWidthWithWindow, mResizeHeightWithWindow;
		bool mIsDirty;

		GUILayout* mLayout;

		GUIArea(GUIWidget& widget, CM::UINT32 x, CM::UINT32 y, CM::UINT32 width, CM::UINT32 height, CM::UINT16 depth);

		bool isDirty() const;

		void notifyWindowResized(CM::UINT32 newWidth, CM::UINT32 newHeight);

		static void destroyInternal(GUIArea* area);
	};
}