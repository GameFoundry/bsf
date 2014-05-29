#pragma once

#include "BsPrerequisites.h"
#include "BsGUIWidget.h"
#include "BsVector2I.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	struct BS_EXPORT GUIDropDownData
	{
		Vector<GUIDropDownDataEntry> entries;
		UnorderedMap<WString, HString> localizedNames;
	};

	class BS_EXPORT GUIDropDownDataEntry
	{
		enum class Type
		{
			Separator,
			Entry,
			SubMenu
		};

	public:
		static GUIDropDownDataEntry separator();
		static GUIDropDownDataEntry button(const WString& label, std::function<void()> callback);
		static GUIDropDownDataEntry subMenu(const WString& label, const GUIDropDownData& data);

		bool isSeparator() const { return mType == Type::Separator; }
		bool isSubMenu() const { return mType == Type::SubMenu; }

		const WString& getLabel() const { return mLabel; }
		std::function<void()> getCallback() const { return mCallback; }
		const GUIDropDownData& getSubMenuData() const { return mChildData; }
	private:
		GUIDropDownDataEntry() { }

		std::function<void()> mCallback;
		GUIDropDownData mChildData;
		WString mLabel;
		Type mType; 
	};

	/**
	 * @brief	Determines how will the drop down box be placed. Usually the system will attempt to position
	 * 			the drop box in a way so all elements can fit, and this class allows you to specify some limitations
	 * 			on how that works. 
	 * 			
	 * @note	For example, list boxes usually want drop down boxes to be placed above or below them, while
	 * 			context menus may want to have them placed around a single point in any direction.
	 */
	class BS_EXPORT GUIDropDownAreaPlacement
	{
	public:
		enum class Type
		{
			Position,
			BoundsVert,
			BoundsHorz
		};

		/**
		 * @brief	Drop down box will be placed at the specified position. By default the system
		 * 			prefers the top left corner of the box to correspond to the position, but if
		 * 			other corners offer more space for the contents, those will be used instead.
		 */
		static GUIDropDownAreaPlacement aroundPosition(const Vector2I& position);

		/**
		 * @brief	Drop down box will be placed at the specified bounds. Box will be horizontally aligned to the left
		 * 			of the provided bounds. Vertically system prefers placing the box at the bottom of the bounds, but may choose
		 * 			to align it with the top of the bounds if it offers more space for the contents.
		 */
		static GUIDropDownAreaPlacement aroundBoundsVert(const RectI& bounds);
		
		/**
		 * @brief	Drop down box will be placed at the specified bounds. Box will be vertically aligned to the top
		 * 			of the provided bounds. Horizontally system prefers placing the box at the right of the bounds, but may choose
		 * 			to align it with the left of the bounds if it offers more space for the contents.
		 */
		static GUIDropDownAreaPlacement aroundBoundsHorz(const RectI& bounds);

		Type getType() const { return mType; }
		const RectI& getBounds() const { return mBounds; }
		const Vector2I& getPosition() const { return mPosition; }

	private:
		GUIDropDownAreaPlacement() { }

		Type mType;
		RectI mBounds;
		Vector2I mPosition;
	};

	enum class GUIDropDownType
	{
		ListBox,
		ContextMenu,
		MenuBar
	};

	/**
	 * @brief	This is a generic GUI drop down box class that can be used for:
	 * 			list boxes, menu bars or context menus.
	 */
	class BS_EXPORT GUIDropDownBox : public GUIWidget
	{
	public:
		GUIDropDownBox(const HSceneObject& parent, Viewport* target, const GUIDropDownAreaPlacement& placement,
			const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type);
		~GUIDropDownBox();

	private:
		struct DropDownSubMenu
		{
			GUIDropDownBox* mOwner;

			GUIDropDownType mType;
			GUIDropDownData mData;
			UINT32 mPage;
			INT32 x, y;
			UINT32 width, height;
			RectI mVisibleBounds;
			RectI mAvailableBounds;
			UINT32 mDepthOffset;
			bool mOpenedUpward;

			Vector<GUITexture*> mCachedSeparators;
			Vector<GUIButton*> mCachedEntryBtns;
			Vector<GUIButton*> mCachedExpEntryBtns;
			GUIButton* mScrollUpBtn;
			GUIButton* mScrollDownBtn;
			GUITexture* mBackgroundFrame;

			GUIArea* mBackgroundArea;
			GUIArea* mContentArea;
			GUILayout* mContentLayout;

			DropDownSubMenu* mSubMenu;

			DropDownSubMenu(GUIDropDownBox* owner, const GUIDropDownAreaPlacement& placement, 
				const RectI& availableBounds, const GUIDropDownData& dropDownData, GUIDropDownType type, UINT32 depthOffset);
			~DropDownSubMenu();

			void updateGUIElements();

			void scrollDown();
			void scrollUp();

			UINT32 getElementHeight(UINT32 idx) const;

			void elementClicked(UINT32 idx);
			void openSubMenu(GUIButton* source, UINT32 elementIdx);
			void closeSubMenu();

			RectI getVisibleBounds() const { return mVisibleBounds; }

			HString getElementLocalizedName(UINT32 idx) const;
		};

		static const UINT32 DROP_DOWN_BOX_WIDTH;

		String mScrollUpStyle;
		String mScrollDownStyle;
		String mEntryBtnStyle;
		String mEntryExpBtnStyle;
		String mSeparatorStyle;
		String mBackgroundStyle;
		HSpriteTexture mScrollUpBtnArrow;
		HSpriteTexture mScrollDownBtnArrow;

		DropDownSubMenu* mRootMenu;
		GUIDropDownHitBox* mHitBox;
		// Captures mouse clicks so that we don't trigger elements outside the drop down box when we just want to close it.
		// (Particular example is clicking on the button that opened the drop down box in the first place. Clicking will cause
		// the drop down to lose focus and close, but if the button still processes the mouse click it will be immediately opened again)
		GUIDropDownHitBox* mCaptureHitBox; 

		UnorderedMap<WString, HString> mLocalizedEntryNames;

		void notifySubMenuOpened(DropDownSubMenu* subMenu);
		void notifySubMenuClosed(DropDownSubMenu* subMenu);

		void dropDownFocusLost();
	};
}