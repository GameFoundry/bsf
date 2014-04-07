#pragma once

#include "BsPrerequisites.h"
#include "BsGUISkin.h"
#include "CmModule.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	// TODO - Currently this class will always import resources, but it would be better if it first
	// checks for a pre-processed asset and only import it if that doesn't exist
	class BS_EXPORT BuiltinResources : public CamelotFramework::Module<BuiltinResources>
	{
	public:
		BuiltinResources();

		const GUISkin& getSkin() const { return mSkin; }

		const HSpriteTexture getWhiteSpriteTexture() const { return mWhiteSpriteTexture; }

		const CM::PixelData& getCursorArrow(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorArrowDrag(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorWait(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorIBeam(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorSizeNESW(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorSizeNS(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorSizeNWSE(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorSizeWE(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorDeny(CM::Vector2I& hotSpot);
		const CM::PixelData& getCursorMoveLeftRight(CM::Vector2I& hotSpot);

	private:
		GUISkin mSkin;

		CM::PixelDataPtr mCursorArrow;
		CM::PixelDataPtr mCursorArrowDrag;
		CM::PixelDataPtr mCursorArrowLeftRight;
		CM::PixelDataPtr mCursorIBeam;
		CM::PixelDataPtr mCursorDeny;
		CM::PixelDataPtr mCursorWait;
		CM::PixelDataPtr mCursorSizeNESW;
		CM::PixelDataPtr mCursorSizeNS;
		CM::PixelDataPtr mCursorSizeNWSE;
		CM::PixelDataPtr mCursorSizeWE;

		HSpriteTexture mWhiteSpriteTexture;

		static const CM::WString DefaultSkinFolder;
		static const CM::WString DefaultCursorFolder;

		static const CM::WString DefaultFontPath;
		static const CM::UINT32 DefaultFontSize;

		static const CM::WString WhiteTex;

		static const CM::WString ButtonNormalTex;
		static const CM::WString ButtonHoverTex;

		static const CM::WString ToggleNormalTex;
		static const CM::WString ToggleHoverTex;
		static const CM::WString ToggleActiveTex;
		static const CM::WString ToggleNormalOnTex;
		static const CM::WString ToggleHoverOnTex;
		static const CM::WString ToggleActiveOnTex;

		static const CM::WString InputBoxNormalTex;
		static const CM::WString InputBoxHoverTex;
		static const CM::WString InputBoxFocusedTex;

		static const CM::WString ScrollBarUpNormalTex;
		static const CM::WString ScrollBarUpHoverTex;
		static const CM::WString ScrollBarUpActiveTex;

		static const CM::WString ScrollBarDownNormalTex;
		static const CM::WString ScrollBarDownHoverTex;
		static const CM::WString ScrollBarDownActiveTex;

		static const CM::WString ScrollBarLeftNormalTex;
		static const CM::WString ScrollBarLeftHoverTex;
		static const CM::WString ScrollBarLeftActiveTex;

		static const CM::WString ScrollBarRightNormalTex;
		static const CM::WString ScrollBarRightHoverTex;
		static const CM::WString ScrollBarRightActiveTex;

		static const CM::WString ScrollBarHandleHorzNormalTex;
		static const CM::WString ScrollBarHandleHorzHoverTex;
		static const CM::WString ScrollBarHandleHorzActiveTex;

		static const CM::WString ScrollBarHandleVertNormalTex;
		static const CM::WString ScrollBarHandleVertHoverTex;
		static const CM::WString ScrollBarHandleVertActiveTex;

		static const CM::WString ScrollBarBgTex;

		static const CM::WString DropDownBtnNormalTex;
		static const CM::WString DropDownBtnHoverTex;

		static const CM::WString DropDownBoxBgTex;
		static const CM::WString DropDownBoxEntryNormalTex;
		static const CM::WString DropDownBoxEntryHoverTex;

		static const CM::WString DropDownBoxBtnUpNormalTex;
		static const CM::WString DropDownBoxBtnUpHoverTex;

		static const CM::WString DropDownBoxBtnDownNormalTex;
		static const CM::WString DropDownBoxBtnDownHoverTex;

		static const CM::WString DropDownBoxEntryExpNormalTex;
		static const CM::WString DropDownBoxEntryExpHoverTex;

		static const CM::WString DropDownSeparatorTex;

		static const CM::WString DropDownBoxBtnUpArrowTex;
		static const CM::WString DropDownBoxBtnDownArrowTex;

		static const CM::WString CursorArrowTex;
		static const CM::WString CursorArrowDragTex;
		static const CM::WString CursorArrowLeftRightTex;
		static const CM::WString CursorIBeamTex;
		static const CM::WString CursorDenyTex;
		static const CM::WString CursorWaitTex;
		static const CM::WString CursorSizeNESWTex;
		static const CM::WString CursorSizeNSTex;
		static const CM::WString CursorSizeNWSETex;
		static const CM::WString CursorSizeWETex;

		static const CM::Vector2I CursorArrowHotspot;
		static const CM::Vector2I CursorArrowDragHotspot;
		static const CM::Vector2I CursorArrowLeftRightHotspot;
		static const CM::Vector2I CursorIBeamHotspot;
		static const CM::Vector2I CursorDenyHotspot;
		static const CM::Vector2I CursorWaitHotspot;
		static const CM::Vector2I CursorSizeNESWHotspot;
		static const CM::Vector2I CursorSizeNSHotspot;
		static const CM::Vector2I CursorSizeNWSEHotspot;
		static const CM::Vector2I CursorSizeWEHotspot;

		static HSpriteTexture getSkinTexture(const CM::WString& name);
		static CM::HTexture getCursorTexture(const CM::WString& name);
	};
}