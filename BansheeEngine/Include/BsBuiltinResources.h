#pragma once

#include "BsPrerequisites.h"
#include "BsGUISkin.h"
#include "CmModule.h"
#include "CmPath.h"
#include "CmVector2I.h"

namespace BansheeEngine
{
	// TODO - Currently this class will always import resources, but it would be better if it first
	// checks for a pre-processed asset and only import it if that doesn't exist
	class BS_EXPORT BuiltinResources : public BansheeEngine::Module<BuiltinResources>
	{
	public:
		BuiltinResources();
		~BuiltinResources();

		const GUISkin& getSkin() const { return mSkin; }

		const HSpriteTexture getWhiteSpriteTexture() const { return mWhiteSpriteTexture; }

		const PixelData& getCursorArrow(Vector2I& hotSpot);
		const PixelData& getCursorArrowDrag(Vector2I& hotSpot);
		const PixelData& getCursorWait(Vector2I& hotSpot);
		const PixelData& getCursorIBeam(Vector2I& hotSpot);
		const PixelData& getCursorSizeNESW(Vector2I& hotSpot);
		const PixelData& getCursorSizeNS(Vector2I& hotSpot);
		const PixelData& getCursorSizeNWSE(Vector2I& hotSpot);
		const PixelData& getCursorSizeWE(Vector2I& hotSpot);
		const PixelData& getCursorDeny(Vector2I& hotSpot);
		const PixelData& getCursorMoveLeftRight(Vector2I& hotSpot);

	private:
		GUISkin mSkin;

		PixelDataPtr mCursorArrow;
		PixelDataPtr mCursorArrowDrag;
		PixelDataPtr mCursorArrowLeftRight;
		PixelDataPtr mCursorIBeam;
		PixelDataPtr mCursorDeny;
		PixelDataPtr mCursorWait;
		PixelDataPtr mCursorSizeNESW;
		PixelDataPtr mCursorSizeNS;
		PixelDataPtr mCursorSizeNWSE;
		PixelDataPtr mCursorSizeWE;

		HSpriteTexture mWhiteSpriteTexture;

		static const Path DefaultSkinFolder;
		static const Path DefaultCursorFolder;

		static const WString DefaultFontPath;
		static const UINT32 DefaultFontSize;

		static const WString WhiteTex;

		static const WString ButtonNormalTex;
		static const WString ButtonHoverTex;
		static const WString ButtonActiveTex;

		static const WString ToggleNormalTex;
		static const WString ToggleHoverTex;
		static const WString ToggleActiveTex;
		static const WString ToggleNormalOnTex;
		static const WString ToggleHoverOnTex;
		static const WString ToggleActiveOnTex;

		static const WString InputBoxNormalTex;
		static const WString InputBoxHoverTex;
		static const WString InputBoxFocusedTex;

		static const WString ScrollBarUpNormalTex;
		static const WString ScrollBarUpHoverTex;
		static const WString ScrollBarUpActiveTex;

		static const WString ScrollBarDownNormalTex;
		static const WString ScrollBarDownHoverTex;
		static const WString ScrollBarDownActiveTex;

		static const WString ScrollBarLeftNormalTex;
		static const WString ScrollBarLeftHoverTex;
		static const WString ScrollBarLeftActiveTex;

		static const WString ScrollBarRightNormalTex;
		static const WString ScrollBarRightHoverTex;
		static const WString ScrollBarRightActiveTex;

		static const WString ScrollBarHandleHorzNormalTex;
		static const WString ScrollBarHandleHorzHoverTex;
		static const WString ScrollBarHandleHorzActiveTex;

		static const WString ScrollBarHandleVertNormalTex;
		static const WString ScrollBarHandleVertHoverTex;
		static const WString ScrollBarHandleVertActiveTex;

		static const WString ScrollBarBgTex;

		static const WString DropDownBtnNormalTex;
		static const WString DropDownBtnHoverTex;

		static const WString DropDownBoxBgTex;
		static const WString DropDownBoxEntryNormalTex;
		static const WString DropDownBoxEntryHoverTex;

		static const WString DropDownBoxBtnUpNormalTex;
		static const WString DropDownBoxBtnUpHoverTex;

		static const WString DropDownBoxBtnDownNormalTex;
		static const WString DropDownBoxBtnDownHoverTex;

		static const WString DropDownBoxEntryExpNormalTex;
		static const WString DropDownBoxEntryExpHoverTex;

		static const WString DropDownSeparatorTex;

		static const WString DropDownBoxBtnUpArrowTex;
		static const WString DropDownBoxBtnDownArrowTex;

		static const WString CursorArrowTex;
		static const WString CursorArrowDragTex;
		static const WString CursorArrowLeftRightTex;
		static const WString CursorIBeamTex;
		static const WString CursorDenyTex;
		static const WString CursorWaitTex;
		static const WString CursorSizeNESWTex;
		static const WString CursorSizeNSTex;
		static const WString CursorSizeNWSETex;
		static const WString CursorSizeWETex;

		static const Vector2I CursorArrowHotspot;
		static const Vector2I CursorArrowDragHotspot;
		static const Vector2I CursorArrowLeftRightHotspot;
		static const Vector2I CursorIBeamHotspot;
		static const Vector2I CursorDenyHotspot;
		static const Vector2I CursorWaitHotspot;
		static const Vector2I CursorSizeNESWHotspot;
		static const Vector2I CursorSizeNSHotspot;
		static const Vector2I CursorSizeNWSEHotspot;
		static const Vector2I CursorSizeWEHotspot;

		static HSpriteTexture getSkinTexture(const WString& name);
		static HTexture getCursorTexture(const WString& name);
	};
}