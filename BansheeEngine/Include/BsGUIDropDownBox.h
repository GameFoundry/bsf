#pragma once

#include "BsPrerequisites.h"
#include "BsGUIWidget.h"
#include "CmInt2.h"
#include <boost/signal.hpp>

namespace BansheeEngine
{
	struct BS_EXPORT GUIDropDownData
	{
		CM::Vector<GUIDropDownDataEntry>::type entries;
		CM::UnorderedMap<CM::WString, CM::HString>::type localizedNames;
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
		static GUIDropDownDataEntry button(const CM::WString& label, std::function<void()> callback);
		static GUIDropDownDataEntry subMenu(const CM::WString& label, const GUIDropDownData& data);

		bool isSeparator() const { return mType == Type::Separator; }
		bool isSubMenu() const { return mType == Type::SubMenu; }

		const CM::WString& getLabel() const { return mLabel; }
		std::function<void()> getCallback() const { return mCallback; }
		const GUIDropDownData& getSubMenuData() const { return mChildData; }
	private:
		GUIDropDownDataEntry() { }

		std::function<void()> mCallback;
		GUIDropDownData mChildData;
		CM::WString mLabel;
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
		static GUIDropDownAreaPlacement aroundPosition(const CM::Int2& position);

		/**
		 * @brief	Drop down box will be placed at the specified bounds. Box will be horizontally aligned to the left
		 * 			of the provided bounds. Vertically system prefers placing the box at the bottom of the bounds, but may choose
		 * 			to align it with the top of the bounds if it offers more space for the contents.
		 */
		static GUIDropDownAreaPlacement aroundBoundsVert(const CM::Rect& bounds);
		
		/**
		 * @brief	Drop down box will be placed at the specified bounds. Box will be vertically aligned to the top
		 * 			of the provided bounds. Horizontally system prefers placing the box at the right of the bounds, but may choose
		 * 			to align it with the left of the bounds if it offers more space for the contents.
		 */
		static GUIDropDownAreaPlacement aroundBoundsHorz(const CM::Rect& bounds);

		Type getType() const { return mType; }
		const CM::Rect& getBounds() const { return mBounds; }
		const CM::Int2& getPosition() const { return mPosition; }

	private:
		GUIDropDownAreaPlacement() { }

		Type mType;
		CM::Rect mBounds;
		CM::Int2 mPosition;
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
		GUIDropDownBox(const CM::HSceneObject& parent, CM::Viewport* target, CM::RenderWindow* window, const GUIDropDownAreaPlacement& placement,
			const GUIDropDownData& dropDownData, const GUISkin& skin, GUIDropDownType type);
		~GUIDropDownBox();

	private:
		struct DropDownSubMenu
		{
			GUIDropDownBox* mOwner;

			GUIDropDownType mType;
			GUIDropDownData mData;
			CM::UINT32 mPage;
			CM::INT32 x, y;
			CM::UINT32 width, height;
			CM::Rect mAvailableBounds;
			CM::UINT32 mDepthOffset;
			bool mOpenedUpward;

			CM::Vector<GUITexture*>::type mCachedSeparators;
			CM::Vector<GUIButton*>::type mCachedEntryBtns;
			CM::Vector<GUIButton*>::type mCachedExpEntryBtns;
			GUIButton* mScrollUpBtn;
			GUIButton* mScrollDownBtn;
			GUITexture* mBackgroundFrame;

			GUIArea* mBackgroundArea;
			GUIArea* mContentArea;
			GUILayout* mContentLayout;

			DropDownSubMenu* mSubMenu;

			DropDownSubMenu(GUIDropDownBox* owner, const GUIDropDownAreaPlacement& placement, 
				const CM::Rect& availableBounds, const GUIDropDownData& dropDownData, GUIDropDownType type, CM::UINT32 depthOffset);
			~DropDownSubMenu();

			void updateGUIElements();

			void scrollDown();
			void scrollUp();

			CM::UINT32 getElementHeight(CM::UINT32 idx) const;

			void elementClicked(CM::UINT32 idx);
			void openSubMenu(GUIButton* source, CM::UINT32 elementIdx);
			void closeSubMenu();

			CM::HString getElementLocalizedName(CM::UINT32 idx) const;
		};

		static const CM::UINT32 DROP_DOWN_BOX_WIDTH;

		const GUIElementStyle* mScrollUpStyle;
		const GUIElementStyle* mScrollDownStyle;
		const GUIElementStyle* mEntryBtnStyle;
		const GUIElementStyle* mEntryExpBtnStyle;
		const GUIElementStyle* mSeparatorStyle;
		const GUIElementStyle* mBackgroundStyle;
		SpriteTexturePtr mScrollUpBtnArrow;
		SpriteTexturePtr mScrollDownBtnArrow;

		DropDownSubMenu* mRootMenu;

		CM::UnorderedMap<CM::WString, CM::HString>::type mLocalizedEntryNames;
	};
}