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
		static GUIArea* create(GUIWidget& widget, CM::INT32 x, CM::INT32 y, CM::UINT32 width = 0, CM::UINT32 height = 0, CM::UINT16 depth = 0);

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			This kind of GUI area will always stretch to completely fill the parent widget, while respecting the
		 *			provided offsets. 
		 */
		static GUIArea* createStretchedXY(GUIWidget& widget, CM::UINT32 offsetLeft, 
			CM::UINT32 offsetRight, CM::UINT32 offsetTop, CM::UINT32 offsetBottom, CM::UINT16 depth = 0);

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			This kind of GUI area will always stretch in X dimension to completely fill the parent widget, while respecting the
		 *			provided offsets. 
		 */
		static GUIArea* createStretchedX(GUIWidget& widget, CM::UINT32 offsetLeft, 
			CM::UINT32 offsetRight, CM::UINT32 offsetTop, CM::UINT32 height, CM::UINT16 depth = 0);

		/**
		 * @brief	Sets up a new GUI area. All the layouts used in the area will be placed
		 * 			within the specified bounds.
		 * 			
		 *			This kind of GUI area will always stretch in Y dimension to completely fill the parent widget, while respecting the
		 *			provided offsets. 
		 */
		static GUIArea* createStretchedY(GUIWidget& widget, CM::UINT32 offsetTop, 
			CM::UINT32 offsetBottom, CM::UINT32 offsetLeft, CM::UINT32 width, CM::UINT16 depth = 0);

		static void destroy(GUIArea* area);

		GUILayout& getLayout() const { return *mLayout; }

		CM::UINT16 getDepth() const { return mDepth; }
		void setDepth(CM::UINT16 depth) { mDepth = depth; }

		void setPosition(CM::INT32 x, CM::INT32 y);
		void setSize(CM::UINT32 width, CM::UINT32 height);

		void disable();
		void enable();

		void changeParentWidget(GUIWidget* widget);

		CM::INT32 x() const { return mLeft; }
		CM::INT32 y() const { return mTop; }
		CM::UINT32 width() const { return mWidth; }
		CM::UINT32 height() const { return mHeight; }

		void _update();		
	private:
		friend class GUIWidget;

		GUIWidget* mWidget;
		CM::INT32 mLeft, mRight, mTop, mBottom;
		CM::UINT32 mWidth, mHeight;
		CM::UINT16 mDepth;
		bool mResizeXWithWidget;
		bool mResizeYWithWidget;
		bool mIsDirty;
		bool mIsDisabled;

		GUILayout* mLayout;

		GUIArea(GUIWidget* widget, CM::INT32 x, CM::INT32 y, CM::UINT16 depth);

		bool isDirty() const;
		void updateSizeBasedOnParent(CM::UINT32 parentWidth, CM::UINT32 parentHeight);

		static void destroyInternal(GUIArea* area);
	};
}