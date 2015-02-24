#pragma once

#include "BsPrerequisites.h"
#include "BsRect2I.h"

namespace BansheeEngine
{
	/**
	 * @brief	Type of layout that a GUIArea can initially be created with
	 */
	enum class GUILayoutType
	{
		LayoutX, LayoutY, LayoutExplicit
	};

	/**
	 * @brief	GUIArea represents a freely positionable GUI panel with a GUILayout child.
	 *			This is one of the top GUI elements (aside from GUIWidget) and all layouts
	 *			must belong to a single GUIArea.
	 *
	 * @note	GUIAreas are automatically cleaned when their parent GUIWidget is destroyed so you
	 *			dont need to worry about their cleanup.
	 */
	class BS_EXPORT GUIArea
	{
	public:
		~GUIArea();

		/**
		 * Creates a new GUI area. All the layouts used in the area will be placed
		 * within the specified bounds.
		 *
		 * @param	widget		Widget parent of the GUIArea.
		 * @param	x			X position relative to parent widget position, in pixels.
		 * @param	y			Y position relative to parent widget position, in pixels.
		 * @param	width		Width of the area, in pixels. If 0 then the area will be of unlimited width.
		 * @param	height		Height of the area, in pixels. If 0 then the area will be of unlimited height.
		 * @param	depth		Depth of the area. This is relative to the depth of parent widget. Depth determines in what
		 *						order are overlapping GUIAreas drawn. Areas with lower depth will be drawn in front of areas
		 *						with higher depth.
		 * @param	layoutType	Type of layout to create as the root layout for the area.
		 */
		static GUIArea* create(GUIWidget& widget, INT32 x, INT32 y, UINT32 width = 0, UINT32 height = 0, UINT16 depth = 0, 
			GUILayoutType layoutType = GUILayoutType::LayoutX);

		/**
		 * Creates a new GUI area. All the layouts used in the area will be placed
		 * within the specified bounds.
		 * 			
		 * This kind of GUI area will always stretch to completely fill the parent widget, 
		 * while respecting the provided offsets (in pixels). 
		 *
		 * @param	widget			Widget parent of the GUIArea.
		 * @param	offsetLeft		Offset to keep from the left of the parent widget, in pixels.
		 * @param	offsetRight		Offset to keep from the right of the parent widget, in pixels.
		 * @param	offsetTop		Offset to keep from the top of the parent widget, in pixels.
		 * @param	offsetBottom	Offset to keep from the bottom of the parent widget, in pixels.
		 * @param	depth			Depth of the area. This is relative to the depth of parent widget. Depth determines in what
		 *							order are overlapping GUIAreas drawn. Areas with lower depth will be drawn in front of areas
		 *							with higher depth.
		 * @param	layoutType		Type of layout to create as the root layout for the area.
		 */
		static GUIArea* createStretchedXY(GUIWidget& widget, UINT32 offsetLeft, 
			UINT32 offsetRight, UINT32 offsetTop, UINT32 offsetBottom, UINT16 depth = 0,
			GUILayoutType layoutType = GUILayoutType::LayoutX);

		/**
		 * Creates a new GUI area. All the layouts used in the area will be placed
		 * within the specified bounds.
		 * 			
		 * This kind of GUI area will always stretch to completely fill the parent widget width, 
		 * while respecting the provided offsets (in pixels). 
		 *
		 * @param	widget			Widget parent of the GUIArea.
		 * @param	offsetLeft		Offset to keep from the left of the parent widget, in pixels.
		 * @param	offsetRight		Offset to keep from the right of the parent widget, in pixels.
		 * @param	y				Y position relative to parent widget position, in pixels.
		 * @param	height			Height of the area, in pixels. If 0 then the area will be of unlimited height.
		 * @param	depth			Depth of the area. This is relative to the depth of parent widget. Depth determines in what
		 *							order are overlapping GUIAreas drawn. Areas with lower depth will be drawn in front of areas
		 *							with higher depth.
		 * @param	layoutType		Type of layout to create as the root layout for the area.
		 */
		static GUIArea* createStretchedX(GUIWidget& widget, UINT32 offsetLeft, 
			UINT32 offsetRight, UINT32 y, UINT32 height, UINT16 depth = 0,
			GUILayoutType layoutType = GUILayoutType::LayoutX);

		/**
		 * Creates a new GUI area. All the layouts used in the area will be placed
		 * within the specified bounds.
		 * 			
		 * This kind of GUI area will always stretch to completely fill the parent widget height, 
		 * while respecting the provided offsets (in pixels). 
		 *
		 * @param	widget			Widget parent of the GUIArea.
		 * @param	offsetTop		Offset to keep from the top of the parent widget, in pixels.
		 * @param	offsetBottom	Offset to keep from the bottom of the parent widget, in pixels.
		 * @param	x				X position relative to parent widget position, in pixels.
		 * @param	width			Width of the area, in pixels. If 0 then the area will be of unlimited width.
		 * @param	depth			Depth of the area. This is relative to the depth of parent widget. Depth determines in what
		 *							order are overlapping GUIAreas drawn. Areas with lower depth will be drawn in front of areas
		 *							with higher depth.
		 * @param	layoutType		Type of layout to create as the root layout for the area.
		 */
		static GUIArea* createStretchedY(GUIWidget& widget, UINT32 offsetTop, 
			UINT32 offsetBottom, UINT32 x, UINT32 width, UINT16 depth = 0,
			GUILayoutType layoutType = GUILayoutType::LayoutX);

		/**
		 * @brief	Destroys a GUI area and removes it from the parent GUIWidget.
		 *			Any child layouts are also destroyed so make sure you aren't 
		 *			not referencing any of them.
		 *
		 * @note	Attached GUIElements are not destroyed, you must destroy them manually or
		 *			they will be destroyed when parent GUIWidget is destroyed.
		 */
		static void destroy(GUIArea* area);

		/**
		 * @brief	Returns primary layout for the GUI area. Use this to
		 *			attach GUI elements or other layouts.
		 */
		GUILayout& getLayout() const { return *mLayout; }

		/**
		 * @brief	Returns depth of the GUI area. This is relative to the depth of parent widget. 
		 *			Depth determines in what order are overlapping GUIAreas drawn. Areas with lower 
		 *			depth will be drawn in front of areas with higher depth.
		 */
		UINT16 getDepth() const { return mDepth; }

		/**
		 * @brief	Sets depth of the GUI area. This is relative to the depth of parent widget. 
		 *			Depth determines in what order are overlapping GUIAreas drawn. Areas with lower 
		 *			depth will be drawn in front of areas with higher depth.
		 */
		void setDepth(UINT16 depth) { mDepth = depth; }

		/**
		 * @brief	Changes the position of the GUI area relative to the parent widget, in pixels.
		 */
		void setPosition(INT32 x, INT32 y);

		/**
		 * @brief	Changes the size of the GUI area, in pixels. If sizes are zero, size is
		 *			considered to be unlimited.
		 */
		void setSize(UINT32 width, UINT32 height);

		/**
		 * @brief	Sets a rectangle that will be used for clipping of the area contents.
		 *
		 * @param	clipRect	Rectangle in coordinates relative to the area. If width or height
		 *						is zero, no clipping will be done.
		 */
		void setClipRect(const Rect2I& clipRect);

		/**
		 * @brief	Gets a rectangle that is used for clipping of the area contents.
		 */
		Rect2I getClipRect() const { return mClipRect; }

		/**
		 * @brief	Hides the area and any child elements.
		 */
		void disable();

		/**
		 * @brief	Shows the area and any child elements.
		 */
		void enable();

		/**
		 * @brief	Changes the parent widget of the GUI area. This may be null
		 *			but caller is then responsible for properly cleaning up the area.
		 *			(Since it's normally done by the parent widget). Areas will
		 *			null parent won't be rendered.
		 */
		void changeParentWidget(GUIWidget* widget);

		/**
		 * @brief	X position of the GUI area, in pixels relative to parent widget.
		 */
		INT32 x() const { return mLeft; }

		/**
		 * @brief	Y position of the GUI area, in pixels relative to parent widget.
		 */
		INT32 y() const { return mTop; }

		/**
		 * @brief	Width of the GUI area, in pixels. If zero with is unlimited.
		 */
		UINT32 width() const { return mWidth; }

		/**
		 * @brief	Height of the GUI area, in pixels. If zero with is unlimited.
		 */
		UINT32 height() const { return mHeight; }

		/**
		 * @brief	Called once per frame. Updates any child layouts if dirty.
		 *
		 * @note	Internal method.
		 */
		void _update();		
	private:
		friend class GUIWidget;

		/**
		 * @brief	Constructs the area with the specified parent widget (cannot be null),
		 *			position relative to the parent widget in pixels, and depth relative to
		 *			parent widget (smaller depth means closer).
		 */
		GUIArea(GUIWidget* widget, INT32 x, INT32 y, UINT16 depth, GUILayoutType layoutType);

		/**
		 * @brief	Check if any of the area properties changed since the last update.
		 */
		bool isDirty() const;

		/**
		 * @brief	Update area size based on parent size and set offsets (if area is set to be stretched).
		 */
		void updateSizeBasedOnParent(UINT32 parentWidth, UINT32 parentHeight);

		/**
		 * @brief	Destroys the area without unregistering it from the parent (assumed to have done that earlier).
		 */
		static void destroyInternal(GUIArea* area);

	private:
		GUIWidget* mWidget;
		INT32 mLeft, mRight, mTop, mBottom;
		UINT32 mWidth, mHeight;
		UINT16 mDepth;
		Rect2I mClipRect;
		bool mResizeXWithWidget;
		bool mResizeYWithWidget;
		bool mIsDirty;
		bool mIsDisabled;

		GUILayout* mLayout;
	};
}