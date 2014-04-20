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
		 */
		static GUIArea* create(GUIWidget& widget, INT32 x, INT32 y, UINT32 width = 0, UINT32 height = 0, UINT16 depth = 0);

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			This kind of GUI area will always stretch to completely fill the parent widget, while respecting the
		 *			provided offsets. 
		 */
		static GUIArea* createStretchedXY(GUIWidget& widget, UINT32 offsetLeft, 
			UINT32 offsetRight, UINT32 offsetTop, UINT32 offsetBottom, UINT16 depth = 0);

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			This kind of GUI area will always stretch in X dimension to completely fill the parent widget, while respecting the
		 *			provided offsets. 
		 */
		static GUIArea* createStretchedX(GUIWidget& widget, UINT32 offsetLeft, 
			UINT32 offsetRight, UINT32 offsetTop, UINT32 height, UINT16 depth = 0);

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			This kind of GUI area will always stretch in Y dimension to completely fill the parent widget, while respecting the
		 *			provided offsets. 
		 */
		static GUIArea* createStretchedY(GUIWidget& widget, UINT32 offsetTop, 
			UINT32 offsetBottom, UINT32 offsetLeft, UINT32 width, UINT16 depth = 0);

		static void destroy(GUIArea* area);

		GUILayout& getLayout() const { return *mLayout; }

		UINT16 getDepth() const { return mDepth; }
		void setDepth(UINT16 depth) { mDepth = depth; }

		void setPosition(INT32 x, INT32 y);
		void setSize(UINT32 width, UINT32 height);

		void disable();
		void enable();

		void changeParentWidget(GUIWidget* widget);

		INT32 x() const { return mLeft; }
		INT32 y() const { return mTop; }
		UINT32 width() const { return mWidth; }
		UINT32 height() const { return mHeight; }

		void _update();		
	private:
		friend class GUIWidget;

		GUIWidget* mWidget;
		INT32 mLeft, mRight, mTop, mBottom;
		UINT32 mWidth, mHeight;
		UINT16 mDepth;
		bool mResizeXWithWidget;
		bool mResizeYWithWidget;
		bool mIsDirty;
		bool mIsDisabled;

		GUILayout* mLayout;

		GUIArea(GUIWidget* widget, INT32 x, INT32 y, UINT16 depth);

		bool isDirty() const;
		void updateSizeBasedOnParent(UINT32 parentWidth, UINT32 parentHeight);

		static void destroyInternal(GUIArea* area);
	};
}