#include "BsBuiltinResources.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsGUIToggle.h"
#include "BsTextSprite.h"
#include "BsSpriteTexture.h"

#include "BsFont.h"
#include "BsFontImportOptions.h"
#include "BsGpuProgramImportOptions.h"
#include "BsImporter.h"
#include "BsResources.h"
#include "BsGpuProgram.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsMaterial.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRTTIType.h"
#include "BsFileSystem.h"
#include "BsCoreApplication.h"
#include "BsCoreThread.h"
#include "BsApplication.h"

namespace BansheeEngine
{
	const WString BuiltinResources::DefaultFontFilename = L"arial.ttf";
	const UINT32 BuiltinResources::DefaultFontSize = 10;

	const Path BuiltinResources::DefaultSkinFolderRaw = L"..\\..\\..\\..\\Data\\Raw\\Engine\\Skin\\";
	const Path BuiltinResources::DefaultCursorFolderRaw = L"..\\..\\..\\..\\Data\\Raw\\Engine\\Cursors\\";
	const Path BuiltinResources::DefaultShaderFolderRaw = L"..\\..\\..\\..\\Data\\Raw\\Engine\\Shaders\\";

	const Path BuiltinResources::DefaultSkinFolder = L"..\\..\\..\\..\\Data\\Engine\\Skin\\";
	const Path BuiltinResources::DefaultCursorFolder = L"..\\..\\..\\..\\Data\\Engine\\Cursors\\";
	const Path BuiltinResources::DefaultShaderFolder = L"..\\..\\..\\..\\Data\\Engine\\Shaders\\";

	const WString BuiltinResources::HLSL11ShaderSubFolder = L"HLSL11/";
	const WString BuiltinResources::HLSL9ShaderSubFolder = L"HLSL9/";
	const WString BuiltinResources::GLSLShaderSubFolder = L"GLSL/";

	/************************************************************************/
	/* 								GUI TEXTURES                      		*/
	/************************************************************************/

	const WString BuiltinResources::WhiteTex = L"White.psd";

	const WString BuiltinResources::ButtonNormalTex = L"ButtonNormal.psd";
	const WString BuiltinResources::ButtonHoverTex = L"ButtonHover.psd";
	const WString BuiltinResources::ButtonActiveTex = L"ButtonActive.psd";

	const WString BuiltinResources::ToggleNormalTex = L"ToggleNormal.psd";
	const WString BuiltinResources::ToggleHoverTex = L"ToggleHover.psd";
	const WString BuiltinResources::ToggleActiveTex = L"ToggleActive.psd";
	const WString BuiltinResources::ToggleNormalOnTex = L"ToggleOnNormal.psd";
	const WString BuiltinResources::ToggleHoverOnTex = L"ToggleOnHover.psd";
	const WString BuiltinResources::ToggleActiveOnTex = L"ToggleOnActive.psd";

	const WString BuiltinResources::InputBoxNormalTex = L"InputBoxNormal.psd";
	const WString BuiltinResources::InputBoxHoverTex = L"InputBoxHover.psd";
	const WString BuiltinResources::InputBoxFocusedTex = L"InputBoxFocused.psd";

	const WString BuiltinResources::ScrollBarUpNormalTex = L"ScrollBarUpNormal.psd";
	const WString BuiltinResources::ScrollBarUpHoverTex = L"ScrollBarUpHover.psd";
	const WString BuiltinResources::ScrollBarUpActiveTex = L"ScrollBarUpActive.psd";

	const WString BuiltinResources::ScrollBarDownNormalTex = L"ScrollBarDownNormal.psd";
	const WString BuiltinResources::ScrollBarDownHoverTex = L"ScrollBarDownHover.psd";
	const WString BuiltinResources::ScrollBarDownActiveTex = L"ScrollBarDownActive.psd";

	const WString BuiltinResources::ScrollBarLeftNormalTex = L"ScrollBarLeftNormal.psd";
	const WString BuiltinResources::ScrollBarLeftHoverTex = L"ScrollBarLeftHover.psd";
	const WString BuiltinResources::ScrollBarLeftActiveTex = L"ScrollBarLeftActive.psd";

	const WString BuiltinResources::ScrollBarRightNormalTex = L"ScrollBarRightNormal.psd";
	const WString BuiltinResources::ScrollBarRightHoverTex = L"ScrollBarRightHover.psd";
	const WString BuiltinResources::ScrollBarRightActiveTex = L"ScrollBarRightActive.psd";

	const WString BuiltinResources::ScrollBarHandleHorzNormalTex = L"ScrollBarHorzHandleNormal.psd";
	const WString BuiltinResources::ScrollBarHandleHorzHoverTex = L"ScrollBarHorzHandleHover.psd";
	const WString BuiltinResources::ScrollBarHandleHorzActiveTex = L"ScrollBarHorzHandleActive.psd";

	const WString BuiltinResources::ScrollBarHandleVertNormalTex = L"ScrollBarVertHandleNormal.psd";
	const WString BuiltinResources::ScrollBarHandleVertHoverTex = L"ScrollBarVertHandleHover.psd";
	const WString BuiltinResources::ScrollBarHandleVertActiveTex = L"ScrollBarVertHandleActive.psd";

	const WString BuiltinResources::DropDownBtnNormalTex = L"DropDownNormal.psd";
	const WString BuiltinResources::DropDownBtnHoverTex = L"DropDownHover.psd";

	const WString BuiltinResources::DropDownBoxBgTex = L"DropDownBoxBg.psd";
	const WString BuiltinResources::DropDownBoxEntryNormalTex = L"DropDownButtonNormal.psd";
	const WString BuiltinResources::DropDownBoxEntryHoverTex = L"DropDownButtonHover.psd";

	const WString BuiltinResources::DropDownBoxBtnUpNormalTex = L"DropDownBoxBtnUpNormal.psd";
	const WString BuiltinResources::DropDownBoxBtnUpHoverTex = L"DropDownBoxBtnUpHover.psd";

	const WString BuiltinResources::DropDownBoxBtnDownNormalTex = L"DropDownBoxBtnDownNormal.psd";
	const WString BuiltinResources::DropDownBoxBtnDownHoverTex = L"DropDownBoxBtnDownHover.psd";

	const WString BuiltinResources::DropDownBoxEntryExpNormalTex = L"DropDownExpNormal.psd";
	const WString BuiltinResources::DropDownBoxEntryExpHoverTex = L"DropDownExpHover.psd";

	const WString BuiltinResources::DropDownSeparatorTex = L"DropDownSeparator.psd";

	const WString BuiltinResources::DropDownBoxBtnUpArrowTex = L"DropDownBoxBtnUpArrow.psd";
	const WString BuiltinResources::DropDownBoxBtnDownArrowTex = L"DropDownBoxBtnDownArrow.psd";

	const WString BuiltinResources::ScrollBarBgTex = L"ScrollBarBg.psd";

	/************************************************************************/
	/* 							CURSOR TEXTURES                      		*/
	/************************************************************************/

	const WString BuiltinResources::CursorArrowTex = L"Arrow.psd";
	const WString BuiltinResources::CursorArrowDragTex = L"ArrowDrag.psd";
	const WString BuiltinResources::CursorArrowLeftRightTex = L"ArrowLeftRight.psd";
	const WString BuiltinResources::CursorIBeamTex = L"IBeam.psd";
	const WString BuiltinResources::CursorDenyTex = L"Deny.psd";
	const WString BuiltinResources::CursorWaitTex = L"Wait.psd";
	const WString BuiltinResources::CursorSizeNESWTex = L"SizeNESW.psd";
	const WString BuiltinResources::CursorSizeNSTex = L"SizeNS.psd";
	const WString BuiltinResources::CursorSizeNWSETex = L"SizeNWSE.psd";
	const WString BuiltinResources::CursorSizeWETex = L"SizeWE.psd";

	const Vector2I BuiltinResources::CursorArrowHotspot = Vector2I(11, 8);
	const Vector2I BuiltinResources::CursorArrowDragHotspot = Vector2I(11, 8);
	const Vector2I BuiltinResources::CursorArrowLeftRightHotspot = Vector2I(9, 4);
	const Vector2I BuiltinResources::CursorIBeamHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorDenyHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorWaitHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeNESWHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeNSHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeNWSEHotspot = Vector2I(15, 15);
	const Vector2I BuiltinResources::CursorSizeWEHotspot = Vector2I(15, 15);

	/************************************************************************/
	/* 									SHADERS                      		*/
	/************************************************************************/

	struct GpuProgramImportData
	{
		WString filename;
		String entryPoint;
		GpuProgramType type;
		GpuProgramProfile profile;
		String language;
		WString folder;
	};

	const WString BuiltinResources::ShaderSpriteTextVSFile = L"spriteTextVS.gpuprog";
	const WString BuiltinResources::ShaderSpriteTextPSFile = L"spriteTextPS.gpuprog";
	const WString BuiltinResources::ShaderSpriteImageVSFile = L"spriteImageVS.gpuprog";
	const WString BuiltinResources::ShaderSpriteImagePSFile = L"spriteImagePS.gpuprog";
	const WString BuiltinResources::ShaderDebugDraw2DClipSpaceVSFile = L"debugDraw2DClipSpaceVS.gpuprog";
	const WString BuiltinResources::ShaderDebugDraw2DClipSpacePSFile = L"debugDraw2DClipSpacePS.gpuprog";
	const WString BuiltinResources::ShaderDebugDraw2DScreenSpaceVSFile = L"debugDraw2DScreenSpaceVS.gpuprog";
	const WString BuiltinResources::ShaderDebugDraw2DScreenSpacePSFile = L"debugDraw2DScreenSpacePS.gpuprog";
	const WString BuiltinResources::ShaderDebugDraw3DVSFile = L"debugDraw3DVS.gpuprog";
	const WString BuiltinResources::ShaderDebugDraw3DPSFile = L"debugDraw3DPS.gpuprog";
	const WString BuiltinResources::ShaderDockOverlayVSFile = L"dockDropOverlayVS.gpuprog";
	const WString BuiltinResources::ShaderDockOverlayPSFile = L"dockDropOverlayPS.gpuprog";
	const WString BuiltinResources::ShaderDummyVSFile = L"dummyVS.gpuprog";
	const WString BuiltinResources::ShaderDummyPSFile = L"dummyPS.gpuprog";

	BuiltinResources::~BuiltinResources()
	{
		mCursorArrow = nullptr;
		mCursorArrowDrag = nullptr;
		mCursorArrowLeftRight = nullptr;
		mCursorIBeam = nullptr;
		mCursorDeny = nullptr;
		mCursorWait = nullptr;
		mCursorSizeNESW = nullptr;
		mCursorSizeNS = nullptr;
		mCursorSizeNWSE = nullptr;
		mCursorSizeWE = nullptr;
	}

	BuiltinResources::BuiltinResources(RenderSystemPlugin activeRSPlugin)
		:mRenderSystemPlugin(activeRSPlugin)
	{
		switch (activeRSPlugin)
		{
		case RenderSystemPlugin::DX11:
			mActiveShaderSubFolder = HLSL11ShaderSubFolder;
			mActiveRenderSystem = RenderSystemDX11;
			break;
		case RenderSystemPlugin::DX9:
			mActiveShaderSubFolder = HLSL9ShaderSubFolder;
			mActiveRenderSystem = RenderSystemDX9;
			break;
		case RenderSystemPlugin::OpenGL:
			mActiveShaderSubFolder = GLSLShaderSubFolder;
			mActiveRenderSystem = RenderSystemOpenGL;
			break;
		}

		preprocess();

		initSpriteTextShader();
		initSpriteImageShader();
		initDebugDraw2DClipSpaceShader();
		initDebugDraw2DScreenSpaceShader();
		initDebugDraw3DShader();
		initDummyShader();

		mWhiteSpriteTexture = getSkinTexture(WhiteTex);

		/************************************************************************/
		/* 								CURSOR		                     		*/
		/************************************************************************/

		HTexture cursorArrowTex = getCursorTexture(CursorArrowTex);
		HTexture cursorArrowDragTex = getCursorTexture(CursorArrowDragTex);
		HTexture cursorArrowLeftRightTex = getCursorTexture(CursorArrowLeftRightTex);
		HTexture cursorIBeamTex = getCursorTexture(CursorIBeamTex);
		HTexture cursorDenyTex = getCursorTexture(CursorDenyTex);
		HTexture cursorWaitTex = getCursorTexture(CursorWaitTex);
		HTexture cursorSizeNESWTex = getCursorTexture(CursorSizeNESWTex);
		HTexture cursorSizeNSTex = getCursorTexture(CursorSizeNSTex);
		HTexture cursorSizeNWSETex = getCursorTexture(CursorSizeNWSETex);
		HTexture cursorSizeWETex = getCursorTexture(CursorSizeWETex);

		mCursorArrow = cursorArrowTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorArrowTex.getInternalPtr(), 0, mCursorArrow);

		mCursorArrowDrag = cursorArrowDragTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorArrowDragTex.getInternalPtr(), 0, mCursorArrowDrag);

		mCursorArrowLeftRight = cursorArrowLeftRightTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorArrowLeftRightTex.getInternalPtr(), 0, mCursorArrowLeftRight);

		mCursorIBeam = cursorIBeamTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorIBeamTex.getInternalPtr(), 0, mCursorIBeam);

		mCursorDeny = cursorDenyTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorDenyTex.getInternalPtr(), 0, mCursorDeny);

		mCursorWait = cursorWaitTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorWaitTex.getInternalPtr(), 0, mCursorWait);

		mCursorSizeNESW = cursorSizeNESWTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeNESWTex.getInternalPtr(), 0, mCursorSizeNESW);

		mCursorSizeNS = cursorSizeNSTex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeNSTex.getInternalPtr(), 0, mCursorSizeNS);

		mCursorSizeNWSE = cursorSizeNWSETex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeNWSETex.getInternalPtr(), 0, mCursorSizeNWSE);

		mCursorSizeWE = cursorSizeWETex->allocateSubresourceBuffer(0);
		gCoreAccessor().readSubresource(cursorSizeWETex.getInternalPtr(), 0, mCursorSizeWE);

		gCoreAccessor().submitToCoreThread(true);

		Path fontPath = FileSystem::getWorkingDirectoryPath();
		fontPath.append(DefaultSkinFolder);
		fontPath.append(DefaultFontFilename + L".asset");

		HFont font = Resources::instance().load<Font>(fontPath);

		// Label
		GUIElementStyle labelStyle;
		labelStyle.font = font;
		labelStyle.fontSize = DefaultFontSize;
		labelStyle.fixedWidth = false;
		labelStyle.fixedHeight = true;
		labelStyle.height = 11;
		labelStyle.minWidth = 10;

		mSkin.setStyle(GUILabel::getGUITypeName(), labelStyle);

		// Button
		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = getSkinTexture(ButtonNormalTex);
		buttonStyle.hover.texture = getSkinTexture(ButtonHoverTex);
		buttonStyle.active.texture = getSkinTexture(ButtonActiveTex);
		buttonStyle.border.left = 6;
		buttonStyle.border.right = 6;
		buttonStyle.border.top = 6;
		buttonStyle.border.bottom = 6;
		buttonStyle.contentOffset.left = 3;
		buttonStyle.contentOffset.right = 3;
		buttonStyle.fixedHeight = true;
		buttonStyle.height = 15;
		buttonStyle.minWidth = 50;
		buttonStyle.font = font;
		buttonStyle.fontSize = DefaultFontSize;
		buttonStyle.textHorzAlign = THA_Center;
		buttonStyle.textVertAlign = TVA_Center;

		mSkin.setStyle(GUIButton::getGUITypeName(), buttonStyle);

		// Toggle
		GUIElementStyle toggleStyle;
		toggleStyle.normal.texture = getSkinTexture(ToggleNormalTex);
		toggleStyle.hover.texture = getSkinTexture(ToggleHoverTex);
		toggleStyle.active.texture = getSkinTexture(ToggleActiveTex);
		toggleStyle.normalOn.texture = getSkinTexture(ToggleNormalOnTex);
		toggleStyle.hoverOn.texture = getSkinTexture(ToggleHoverOnTex);
		toggleStyle.activeOn.texture = getSkinTexture(ToggleActiveOnTex);
		toggleStyle.fixedHeight = true;
		toggleStyle.fixedWidth = true;
		toggleStyle.height = 15;
		toggleStyle.width = 15;

		mSkin.setStyle(GUIToggle::getGUITypeName(), toggleStyle);

		// Input box
		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = getSkinTexture(InputBoxNormalTex);
		inputBoxStyle.hover.texture = getSkinTexture(InputBoxHoverTex);
		inputBoxStyle.focused.texture = getSkinTexture(InputBoxFocusedTex);
		inputBoxStyle.active.texture = inputBoxStyle.normal.texture;
		inputBoxStyle.border.left = 1;
		inputBoxStyle.border.right = 1;
		inputBoxStyle.border.top = 1;
		inputBoxStyle.border.bottom = 1;
		inputBoxStyle.contentOffset.left = 3;
		inputBoxStyle.contentOffset.right = 3;
		inputBoxStyle.contentOffset.top = 2;
		inputBoxStyle.contentOffset.bottom = 2;
		inputBoxStyle.fixedHeight = true;
		inputBoxStyle.height = 15;
		inputBoxStyle.minWidth = 10;
		inputBoxStyle.font = font;
		inputBoxStyle.fontSize = DefaultFontSize;
		inputBoxStyle.textHorzAlign = THA_Left;
		inputBoxStyle.textVertAlign = TVA_Top;

		mSkin.setStyle(GUIInputBox::getGUITypeName(), inputBoxStyle);

		/************************************************************************/
		/* 								SCROLL BAR                      		*/
		/************************************************************************/

		// Up button
		GUIElementStyle scrollUpBtnStyle;
		scrollUpBtnStyle.normal.texture = getSkinTexture(ScrollBarUpNormalTex);
		scrollUpBtnStyle.hover.texture = getSkinTexture(ScrollBarUpHoverTex);
		scrollUpBtnStyle.active.texture = getSkinTexture(ScrollBarUpActiveTex);
		scrollUpBtnStyle.fixedHeight = true;
		scrollUpBtnStyle.fixedWidth = true;
		scrollUpBtnStyle.height = 4;
		scrollUpBtnStyle.width = 8;

		mSkin.setStyle("ScrollUpBtn", scrollUpBtnStyle);

		// Down button
		GUIElementStyle scrollDownBtnStyle;
		scrollDownBtnStyle.normal.texture = getSkinTexture(ScrollBarDownNormalTex);
		scrollDownBtnStyle.hover.texture = getSkinTexture(ScrollBarDownHoverTex);
		scrollDownBtnStyle.active.texture = getSkinTexture(ScrollBarDownActiveTex);
		scrollDownBtnStyle.fixedHeight = true;
		scrollDownBtnStyle.fixedWidth = true;
		scrollDownBtnStyle.height = 4;
		scrollDownBtnStyle.width = 8;

		mSkin.setStyle("ScrollDownBtn", scrollDownBtnStyle);

		// Left button
		GUIElementStyle scrollLeftBtnStyle;
		scrollLeftBtnStyle.normal.texture = getSkinTexture(ScrollBarLeftNormalTex);
		scrollLeftBtnStyle.hover.texture = getSkinTexture(ScrollBarLeftHoverTex);
		scrollLeftBtnStyle.active.texture = getSkinTexture(ScrollBarLeftActiveTex);
		scrollLeftBtnStyle.fixedHeight = true;
		scrollLeftBtnStyle.fixedWidth = true;
		scrollLeftBtnStyle.height = 8;
		scrollLeftBtnStyle.width = 4;

		mSkin.setStyle("ScrollLeftBtn", scrollLeftBtnStyle);

		// Right button
		GUIElementStyle scrollRightBtnStyle;
		scrollRightBtnStyle.normal.texture = getSkinTexture(ScrollBarRightNormalTex);
		scrollRightBtnStyle.hover.texture = getSkinTexture(ScrollBarRightHoverTex);
		scrollRightBtnStyle.active.texture = getSkinTexture(ScrollBarRightActiveTex);
		scrollRightBtnStyle.fixedHeight = true;
		scrollRightBtnStyle.fixedWidth = true;
		scrollRightBtnStyle.height = 8;
		scrollRightBtnStyle.width = 4;

		mSkin.setStyle("ScrollRightBtn", scrollRightBtnStyle);

		// Horizontal handle
		GUIElementStyle scrollBarHorzBtnStyle;
		scrollBarHorzBtnStyle.normal.texture = getSkinTexture(ScrollBarHandleHorzNormalTex);
		scrollBarHorzBtnStyle.hover.texture = getSkinTexture(ScrollBarHandleHorzHoverTex);
		scrollBarHorzBtnStyle.active.texture = getSkinTexture(ScrollBarHandleHorzActiveTex);
		scrollBarHorzBtnStyle.fixedHeight = true;
		scrollBarHorzBtnStyle.fixedWidth = true;
		scrollBarHorzBtnStyle.height = 6;
		scrollBarHorzBtnStyle.width = 4;

		mSkin.setStyle("ScrollBarHorzBtn", scrollBarHorzBtnStyle);

		// Vertical handle
		GUIElementStyle scrollBarVertBtnStyle;
		scrollBarVertBtnStyle.normal.texture = getSkinTexture(ScrollBarHandleVertNormalTex);
		scrollBarVertBtnStyle.hover.texture = getSkinTexture(ScrollBarHandleVertHoverTex);
		scrollBarVertBtnStyle.active.texture = getSkinTexture(ScrollBarHandleVertActiveTex);
		scrollBarVertBtnStyle.fixedHeight = true;
		scrollBarVertBtnStyle.fixedWidth = true;
		scrollBarVertBtnStyle.height = 4;
		scrollBarVertBtnStyle.width = 6;

		mSkin.setStyle("ScrollBarVertBtn", scrollBarVertBtnStyle);

		HSpriteTexture scrollBarBgPtr = getSkinTexture(ScrollBarBgTex);

		// Vertical scroll bar
		GUIElementStyle vertScrollBarStyle;
		vertScrollBarStyle.normal.texture = scrollBarBgPtr;
		vertScrollBarStyle.hover.texture = scrollBarBgPtr;
		vertScrollBarStyle.active.texture = scrollBarBgPtr;
		vertScrollBarStyle.fixedHeight = false;
		vertScrollBarStyle.fixedWidth = true;
		vertScrollBarStyle.minHeight = 16;
		vertScrollBarStyle.width = 8;

		mSkin.setStyle("ScrollBarVert", vertScrollBarStyle);

		// Horizontal scroll bar
		GUIElementStyle horzScrollBarStyle;
		horzScrollBarStyle.normal.texture = scrollBarBgPtr;
		horzScrollBarStyle.hover.texture = scrollBarBgPtr;
		horzScrollBarStyle.active.texture = scrollBarBgPtr;
		horzScrollBarStyle.fixedHeight = true;
		horzScrollBarStyle.fixedWidth = false;
		horzScrollBarStyle.minWidth = 16;
		horzScrollBarStyle.height = 8;

		mSkin.setStyle("ScrollBarHorz", horzScrollBarStyle);

		/************************************************************************/
		/* 								DROP DOWN BOX                      		*/
		/************************************************************************/

		// ListBox button
		GUIElementStyle dropDownListStyle;
		dropDownListStyle.normal.texture = getSkinTexture(DropDownBtnNormalTex);
		dropDownListStyle.hover.texture = getSkinTexture(DropDownBtnHoverTex);
		dropDownListStyle.active.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.normalOn.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.hoverOn.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.activeOn.texture = dropDownListStyle.hover.texture;
		dropDownListStyle.fixedHeight = true;
		dropDownListStyle.fixedWidth = false;
		dropDownListStyle.height = 13;
		dropDownListStyle.width = 30;
		dropDownListStyle.contentOffset.left = 3;
		dropDownListStyle.contentOffset.right = 11;
		dropDownListStyle.contentOffset.top = 1;
		dropDownListStyle.contentOffset.bottom = 1;
		dropDownListStyle.border.left = 1;
		dropDownListStyle.border.right = 10;
		dropDownListStyle.border.top = 1;
		dropDownListStyle.border.bottom = 1;
		dropDownListStyle.font = font;
		dropDownListStyle.fontSize = DefaultFontSize;
		dropDownListStyle.textHorzAlign = THA_Left;
		dropDownListStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("ListBox", dropDownListStyle);

		// DropDown scroll up button arrow
		GUIElementStyle dropDownScrollUpBtnArrowStyle;
		dropDownScrollUpBtnArrowStyle.normal.texture = getSkinTexture(DropDownBoxBtnUpArrowTex);
		dropDownScrollUpBtnArrowStyle.hover.texture = dropDownScrollUpBtnArrowStyle.normal.texture;
		dropDownScrollUpBtnArrowStyle.active.texture = dropDownScrollUpBtnArrowStyle.hover.texture;
		dropDownScrollUpBtnArrowStyle.fixedHeight = true;
		dropDownScrollUpBtnArrowStyle.fixedWidth = false;
		dropDownScrollUpBtnArrowStyle.height = 7;
		dropDownScrollUpBtnArrowStyle.width = 30;
		dropDownScrollUpBtnArrowStyle.border.left = 1;
		dropDownScrollUpBtnArrowStyle.border.right = 1;
		dropDownScrollUpBtnArrowStyle.border.top = 1;
		dropDownScrollUpBtnArrowStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);
		mSkin.setStyle("MenuBarScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);
		mSkin.setStyle("ContextMenuScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);

		// DropDown scroll up button
		GUIElementStyle dropDownScrollUpBtnStyle;
		dropDownScrollUpBtnStyle.normal.texture = getSkinTexture(DropDownBoxBtnUpNormalTex);
		dropDownScrollUpBtnStyle.hover.texture = getSkinTexture(DropDownBoxBtnUpHoverTex);
		dropDownScrollUpBtnStyle.active.texture = dropDownScrollUpBtnStyle.hover.texture;
		dropDownScrollUpBtnStyle.fixedHeight = true;
		dropDownScrollUpBtnStyle.fixedWidth = false;
		dropDownScrollUpBtnStyle.height = 7;
		dropDownScrollUpBtnStyle.width = 30;
		dropDownScrollUpBtnStyle.border.left = 1;
		dropDownScrollUpBtnStyle.border.right = 1;
		dropDownScrollUpBtnStyle.border.top = 1;
		dropDownScrollUpBtnStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollUpBtn", dropDownScrollUpBtnStyle);
		mSkin.setStyle("MenuBarScrollUpBtn", dropDownScrollUpBtnStyle);
		mSkin.setStyle("ContextMenuScrollUpBtn", dropDownScrollUpBtnStyle);

		// DropDown scroll down button arrow
		GUIElementStyle dropDownScrollDownBtnArrowStyle;
		dropDownScrollDownBtnArrowStyle.normal.texture = getSkinTexture(DropDownBoxBtnDownArrowTex);
		dropDownScrollDownBtnArrowStyle.hover.texture = dropDownScrollDownBtnArrowStyle.normal.texture;
		dropDownScrollDownBtnArrowStyle.active.texture = dropDownScrollDownBtnArrowStyle.hover.texture;
		dropDownScrollDownBtnArrowStyle.fixedHeight = true;
		dropDownScrollDownBtnArrowStyle.fixedWidth = false;
		dropDownScrollDownBtnArrowStyle.height = 7;
		dropDownScrollDownBtnArrowStyle.width = 30;
		dropDownScrollDownBtnArrowStyle.border.left = 1;
		dropDownScrollDownBtnArrowStyle.border.right = 1;
		dropDownScrollDownBtnArrowStyle.border.top = 1;
		dropDownScrollDownBtnArrowStyle.border.bottom = 1;
		
		mSkin.setStyle("ListBoxScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);
		mSkin.setStyle("MenuBarScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);
		mSkin.setStyle("ContextMenuScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);

		// DropDown scroll down button
		GUIElementStyle dropDownScrollDownBtnStyle;
		dropDownScrollDownBtnStyle.normal.texture = getSkinTexture(DropDownBoxBtnDownNormalTex);
		dropDownScrollDownBtnStyle.hover.texture = getSkinTexture(DropDownBoxBtnDownHoverTex);
		dropDownScrollDownBtnStyle.active.texture = dropDownScrollDownBtnStyle.hover.texture;
		dropDownScrollDownBtnStyle.fixedHeight = true;
		dropDownScrollDownBtnStyle.fixedWidth = false;
		dropDownScrollDownBtnStyle.height = 7;
		dropDownScrollDownBtnStyle.width = 30;
		dropDownScrollDownBtnStyle.border.left = 1;
		dropDownScrollDownBtnStyle.border.right = 1;
		dropDownScrollDownBtnStyle.border.top = 1;
		dropDownScrollDownBtnStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxScrollDownBtn", dropDownScrollDownBtnStyle);
		mSkin.setStyle("MenuBarScrollDownBtn", dropDownScrollDownBtnStyle);
		mSkin.setStyle("ContextMenuScrollDownBtn", dropDownScrollDownBtnStyle);

		// DropDown entry button
		GUIElementStyle dropDownEntryBtnStyle;
		dropDownEntryBtnStyle.normal.texture = getSkinTexture(DropDownBoxEntryNormalTex);
		dropDownEntryBtnStyle.hover.texture = getSkinTexture(DropDownBoxEntryHoverTex);
		dropDownEntryBtnStyle.active.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownEntryBtnStyle.fixedHeight = true;
		dropDownEntryBtnStyle.fixedWidth = false;
		dropDownEntryBtnStyle.height = 14;
		dropDownEntryBtnStyle.width = 30;
		dropDownEntryBtnStyle.border.left = 1;
		dropDownEntryBtnStyle.border.right = 1;
		dropDownEntryBtnStyle.border.top = 1;
		dropDownEntryBtnStyle.border.bottom = 1;
		dropDownEntryBtnStyle.font = font;
		dropDownEntryBtnStyle.fontSize = DefaultFontSize;
		dropDownEntryBtnStyle.textHorzAlign = THA_Left;
		dropDownEntryBtnStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("ListBoxEntryBtn", dropDownEntryBtnStyle);
		mSkin.setStyle("MenuBarEntryBtn", dropDownEntryBtnStyle);
		mSkin.setStyle("ContextMenuEntryBtn", dropDownEntryBtnStyle);

		// DropDown entry button with expand
		GUIElementStyle dropDownEntryExpBtnStyle;
		dropDownEntryExpBtnStyle.normal.texture = getSkinTexture(DropDownBoxEntryExpNormalTex);
		dropDownEntryExpBtnStyle.hover.texture = getSkinTexture(DropDownBoxEntryExpHoverTex);
		dropDownEntryExpBtnStyle.active.texture = dropDownEntryExpBtnStyle.hover.texture;
		dropDownEntryExpBtnStyle.fixedHeight = true;
		dropDownEntryExpBtnStyle.fixedWidth = false;
		dropDownEntryExpBtnStyle.height = 14;
		dropDownEntryExpBtnStyle.width = 30;
		dropDownEntryExpBtnStyle.border.left = 1;
		dropDownEntryExpBtnStyle.border.right = 6;
		dropDownEntryExpBtnStyle.border.top = 1;
		dropDownEntryExpBtnStyle.border.bottom = 1;
		dropDownEntryExpBtnStyle.font = font;
		dropDownEntryExpBtnStyle.fontSize = DefaultFontSize;
		dropDownEntryExpBtnStyle.textHorzAlign = THA_Left;
		dropDownEntryExpBtnStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("ListBoxEntryExpBtn", dropDownEntryExpBtnStyle);
		mSkin.setStyle("MenuBarEntryExpBtn", dropDownEntryExpBtnStyle);
		mSkin.setStyle("ContextMenuEntryExpBtn", dropDownEntryExpBtnStyle);

		// DropDown box frame
		GUIElementStyle dropDownBoxStyle;
		dropDownBoxStyle.normal.texture = getSkinTexture(DropDownBoxBgTex);
		dropDownBoxStyle.hover.texture = dropDownEntryBtnStyle.normal.texture;
		dropDownBoxStyle.active.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownBoxStyle.fixedHeight = false;
		dropDownBoxStyle.fixedWidth = false;
		dropDownBoxStyle.border.left = 1;
		dropDownBoxStyle.border.right = 1;
		dropDownBoxStyle.border.top = 1;
		dropDownBoxStyle.border.bottom = 1;
		dropDownBoxStyle.margins.left = 1;
		dropDownBoxStyle.margins.right = 1;
		dropDownBoxStyle.margins.top = 1;
		dropDownBoxStyle.margins.bottom = 1;

		mSkin.setStyle("ListBoxFrame", dropDownBoxStyle);
		mSkin.setStyle("MenuBarFrame", dropDownBoxStyle);
		mSkin.setStyle("ContextMenuFrame", dropDownBoxStyle);

		// Drop down separator
		GUIElementStyle dropDownSeparatorStyle;
		dropDownSeparatorStyle.normal.texture = getSkinTexture(DropDownSeparatorTex);
		dropDownSeparatorStyle.fixedHeight = true;
		dropDownSeparatorStyle.fixedWidth = false;
		dropDownSeparatorStyle.height = 3;
		dropDownSeparatorStyle.width = 30;
		dropDownSeparatorStyle.border.left = 1;
		dropDownSeparatorStyle.border.right = 1;
		dropDownSeparatorStyle.border.top = 1;
		dropDownSeparatorStyle.border.bottom = 1;

		mSkin.setStyle("ListBoxSeparator", dropDownSeparatorStyle);
		mSkin.setStyle("MenuBarSeparator", dropDownSeparatorStyle);
		mSkin.setStyle("ContextMenuSeparator", dropDownSeparatorStyle);
	}

	void BuiltinResources::preprocess()
	{
		static const WString GUI_TEXTURES[] = { WhiteTex, ButtonNormalTex, ButtonHoverTex, ButtonActiveTex, ToggleNormalTex,
			ToggleHoverTex, ToggleActiveTex, ToggleNormalOnTex, ToggleHoverOnTex, ToggleActiveOnTex, InputBoxNormalTex,
			InputBoxHoverTex, InputBoxFocusedTex, ScrollBarUpNormalTex, ScrollBarUpHoverTex, ScrollBarUpActiveTex,
			ScrollBarDownNormalTex, ScrollBarDownHoverTex, ScrollBarDownActiveTex, ScrollBarLeftNormalTex,
			ScrollBarLeftHoverTex, ScrollBarLeftActiveTex, ScrollBarRightNormalTex, ScrollBarRightHoverTex,
			ScrollBarRightActiveTex, ScrollBarHandleHorzNormalTex, ScrollBarHandleHorzHoverTex, ScrollBarHandleHorzActiveTex,
			ScrollBarHandleVertNormalTex, ScrollBarHandleVertHoverTex, ScrollBarHandleVertActiveTex, DropDownBtnNormalTex,
			DropDownBtnHoverTex, DropDownBoxBgTex, DropDownBoxEntryNormalTex, DropDownBoxEntryHoverTex, DropDownBoxBtnUpNormalTex,
			DropDownBoxBtnUpHoverTex, DropDownBoxBtnDownNormalTex, DropDownBoxBtnDownHoverTex, DropDownBoxEntryExpNormalTex, DropDownBoxEntryExpHoverTex,
			DropDownSeparatorTex, DropDownBoxBtnUpArrowTex, DropDownBoxBtnDownArrowTex };

		static const WString CURSOR_TEXTURES[] = { CursorArrowTex, CursorArrowDragTex, CursorArrowLeftRightTex, CursorIBeamTex,
			CursorDenyTex, CursorWaitTex, CursorSizeNESWTex, CursorSizeNSTex, CursorSizeNWSETex, CursorSizeWETex };

		static const GpuProgramImportData GPU_PROGRAM_IMPORT_DATA[] =
		{
			{ ShaderSpriteTextVSFile,				"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "hlsl", HLSL11ShaderSubFolder},
			{ ShaderSpriteTextPSFile,				"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderSpriteImageVSFile,				"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderSpriteImagePSFile,				"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDebugDraw2DClipSpaceVSFile,		"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDebugDraw2DClipSpacePSFile,		"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDebugDraw2DScreenSpaceVSFile,	"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDebugDraw2DScreenSpacePSFile,	"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDebugDraw3DVSFile,				"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDebugDraw3DPSFile,				"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDummyVSFile,					"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderDummyPSFile,					"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "hlsl", HLSL11ShaderSubFolder },
			{ ShaderSpriteTextVSFile,				"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderSpriteTextPSFile,				"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderSpriteImageVSFile,				"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderSpriteImagePSFile,				"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDebugDraw2DClipSpaceVSFile,		"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDebugDraw2DClipSpacePSFile,		"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDebugDraw2DScreenSpaceVSFile,	"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDebugDraw2DScreenSpacePSFile,	"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDebugDraw3DVSFile,				"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDebugDraw3DPSFile,				"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDummyVSFile,					"vs_main",		GPT_VERTEX_PROGRAM,		GPP_VS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderDummyPSFile,					"ps_main",		GPT_FRAGMENT_PROGRAM,	GPP_PS_2_0, "hlsl", HLSL9ShaderSubFolder },
			{ ShaderSpriteTextVSFile,				"main",			GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderSpriteTextPSFile,				"main",			GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderSpriteImageVSFile,				"main",			GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderSpriteImagePSFile,				"main",			GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDebugDraw2DClipSpaceVSFile,		"main",			GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDebugDraw2DClipSpacePSFile,		"main",			GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDebugDraw2DScreenSpaceVSFile,	"main",			GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDebugDraw2DScreenSpacePSFile,	"main",			GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDebugDraw3DVSFile,				"main",			GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDebugDraw3DPSFile,				"main",			GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDummyVSFile,					"main",			GPT_VERTEX_PROGRAM,		GPP_VS_4_0, "glsl", GLSLShaderSubFolder },
			{ ShaderDummyPSFile,					"main",			GPT_FRAGMENT_PROGRAM,	GPP_PS_4_0, "glsl", GLSLShaderSubFolder },
		};

		if (FileSystem::exists(DefaultCursorFolderRaw))
		{
			FileSystem::remove(DefaultCursorFolder);

			for (auto& tex : CURSOR_TEXTURES)
				importCursorTexture(tex);
		}

		if (FileSystem::exists(DefaultSkinFolderRaw))
		{
			FileSystem::remove(DefaultSkinFolder);

			for (auto& tex : GUI_TEXTURES)
				importSkinTexture(tex);

			{
				Path fontPath = FileSystem::getWorkingDirectoryPath();
				fontPath.append(DefaultSkinFolderRaw);
				fontPath.append(DefaultFontFilename);

				ImportOptionsPtr fontImportOptions = Importer::instance().createImportOptions(fontPath);
				if (rtti_is_of_type<FontImportOptions>(fontImportOptions))
				{
					FontImportOptions* importOptions = static_cast<FontImportOptions*>(fontImportOptions.get());

					Vector<UINT32> fontSizes;
					fontSizes.push_back(DefaultFontSize);
					importOptions->setFontSizes(fontSizes);
					importOptions->setAntialiasing(false);
				}

				HFont font = Importer::instance().import(fontPath, fontImportOptions);

				Path outputPath = FileSystem::getWorkingDirectoryPath();
				outputPath.append(DefaultSkinFolder);
				outputPath.append(DefaultFontFilename + L".asset");

				Resources::instance().save(font, outputPath, true);

				// Save font texture pages as well. TODO - Later maybe figure out a more automatic way to do this
				const FontData* fontData = font->getFontDataForSize(DefaultFontSize);

				Path texPageOutputPath = FileSystem::getWorkingDirectoryPath();
				texPageOutputPath.append(DefaultSkinFolder);

				UINT32 pageIdx = 0;
				for (auto tex : fontData->texturePages)
				{
					texPageOutputPath.setFilename(DefaultFontFilename + L"_texpage_" + toWString(pageIdx) + L".asset");
					Resources::instance().save(tex, texPageOutputPath, true);
				}
			}
		}

		if (FileSystem::exists(DefaultShaderFolderRaw))
		{
			Path shaderFolder = DefaultShaderFolder;
			shaderFolder.append(mActiveShaderSubFolder);

			FileSystem::remove(shaderFolder);

			for (auto& importData : GPU_PROGRAM_IMPORT_DATA)
			{
				if (importData.folder != mActiveShaderSubFolder)
					continue;

				Path gpuProgInputLoc = DefaultShaderFolderRaw;
				gpuProgInputLoc.append(importData.folder);
				gpuProgInputLoc.append(importData.filename);

				Path gpuProgOutputLoc = DefaultShaderFolder;
				gpuProgOutputLoc.append(importData.folder);
				gpuProgOutputLoc.append(importData.filename + L".asset");

				ImportOptionsPtr gpuProgImportOptions = Importer::instance().createImportOptions(gpuProgInputLoc);
				if (rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
				{
					GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

					importOptions->setEntryPoint(importData.entryPoint);
					importOptions->setLanguage(importData.language);
					importOptions->setProfile(importData.profile);
					importOptions->setType(importData.type);
				}

				HGpuProgram gpuProgram = Importer::instance().import(gpuProgInputLoc, gpuProgImportOptions);
				Resources::instance().save(gpuProgram, gpuProgOutputLoc, true);
			}
		}

		Resources::instance().unloadAllUnused();
	}

	HSpriteTexture BuiltinResources::getSkinTexture(const WString& name)
	{
		Path texturePath = FileSystem::getWorkingDirectoryPath();
		texturePath.append(DefaultSkinFolder);
		texturePath.append(L"sprite_" + name + L".asset");

		return Resources::instance().load<SpriteTexture>(texturePath);
	}

	HTexture BuiltinResources::getCursorTexture(const WString& name)
	{
		Path cursorPath = FileSystem::getWorkingDirectoryPath();
		cursorPath.append(DefaultCursorFolder);
		cursorPath.append(name + L".asset");

		return Resources::instance().load<Texture>(cursorPath);
	}

	HGpuProgram BuiltinResources::getGpuProgram(const WString& name)
	{
		Path programPath = DefaultShaderFolder;
		programPath.append(mActiveShaderSubFolder);
		programPath.append(name + L".asset");

		return gResources().load<GpuProgram>(programPath);
	}

	void BuiltinResources::importSkinTexture(const WString& name)
	{
		Path texturePath = FileSystem::getWorkingDirectoryPath();
		texturePath.append(DefaultSkinFolderRaw);
		texturePath.append(name);

		Path texOutputPath = FileSystem::getWorkingDirectoryPath();
		texOutputPath.append(DefaultSkinFolder);
		Path spriteTexOutputPath = texOutputPath;
		texOutputPath.append(name + L".asset");
		spriteTexOutputPath.append(L"sprite_" + name + L".asset");

		HTexture tex = Importer::instance().import<Texture>(texturePath);
		Resources::instance().save(tex, texOutputPath, true);

		HSpriteTexture spriteTex = SpriteTexture::create(tex);
		Resources::instance().save(spriteTex, spriteTexOutputPath, true);
	}

	void BuiltinResources::importCursorTexture(const WString& name)
	{
		Path inputPath = FileSystem::getWorkingDirectoryPath();
		inputPath.append(DefaultCursorFolderRaw);
		inputPath.append(name);

		Path ouputPath = FileSystem::getWorkingDirectoryPath();
		ouputPath.append(DefaultCursorFolder);
		ouputPath.append(name + L".asset");

		HTexture tex = Importer::instance().import<Texture>(inputPath);
		Resources::instance().save(tex, ouputPath, true);
	}

	void BuiltinResources::initSpriteTextShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderSpriteTextVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderSpriteTextPSFile);

		mShaderSpriteText = Shader::create("TextSpriteShader");

		mShaderSpriteText->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mShaderSpriteText->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mShaderSpriteText->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);

		if (mRenderSystemPlugin == RenderSystemPlugin::DX11) // TODO: Find a way to avoid this
			mShaderSpriteText->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		else
			mShaderSpriteText->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);

		mShaderSpriteText->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);
		mShaderSpriteText->addParameter("tint", "tint", GPDT_FLOAT4);

		TechniquePtr newTechnique = mShaderSpriteText->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;
		desc.renderTargetDesc[0].renderTargetWriteMask = 0x7; // Don't write to alpha

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void BuiltinResources::initSpriteImageShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderSpriteImageVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderSpriteImagePSFile);

		mShaderSpriteImage = Shader::create("ImageSpriteShader");

		mShaderSpriteImage->addParameter("worldTransform", "worldTransform", GPDT_MATRIX_4X4);
		mShaderSpriteImage->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mShaderSpriteImage->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);

		if (mRenderSystemPlugin == RenderSystemPlugin::DX11) // TODO: Find a way to avoid this
			mShaderSpriteImage->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		else
			mShaderSpriteImage->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);

		mShaderSpriteImage->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);
		mShaderSpriteImage->addParameter("tint", "tint", GPDT_FLOAT4);

		TechniquePtr newTechnique = mShaderSpriteImage->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;
		desc.renderTargetDesc[0].renderTargetWriteMask = 0x7; // Don't write to alpha

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void BuiltinResources::initDebugDraw2DClipSpaceShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderDebugDraw2DClipSpaceVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderDebugDraw2DClipSpacePSFile);

		mShaderDebugDraw2DClipSpace = Shader::create("DebugDraw2DClipSpaceShader");

		TechniquePtr newTechnique = mShaderDebugDraw2DClipSpace->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void BuiltinResources::initDebugDraw2DScreenSpaceShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderDebugDraw2DScreenSpaceVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderDebugDraw2DScreenSpacePSFile);

		mShaderDebugDraw2DScreenSpace = Shader::create("DebugDraw2DScreenSpaceShader");

		mShaderDebugDraw2DScreenSpace->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mShaderDebugDraw2DScreenSpace->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);

		TechniquePtr newTechnique = mShaderDebugDraw2DScreenSpace->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);

		DEPTH_STENCIL_STATE_DESC depthStateDesc;
		depthStateDesc.depthReadEnable = false;
		depthStateDesc.depthWriteEnable = false;

		HDepthStencilState depthState = DepthStencilState::create(depthStateDesc);
		newPass->setDepthStencilState(depthState);
	}

	void BuiltinResources::initDebugDraw3DShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderDebugDraw3DVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderDebugDraw3DPSFile);

		mShaderDebugDraw3D = Shader::create("DebugDraw3DShader");

		mShaderDebugDraw3D->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderDebugDraw3D->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC desc;
		desc.renderTargetDesc[0].blendEnable = true;
		desc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		desc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		desc.renderTargetDesc[0].blendOp = BO_ADD;

		HBlendState blendState = BlendState::create(desc);
		newPass->setBlendState(blendState);
	}

	void BuiltinResources::initDummyShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderDummyVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderDummyPSFile);

		mShaderDummy = Shader::create("DummyShader");

		mShaderDummy->addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderDummy->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);
	}

	const PixelData& BuiltinResources::getCursorArrow(Vector2I& hotSpot)
	{
		hotSpot = CursorArrowHotspot;
		return *mCursorArrow.get();
	}

	const PixelData& BuiltinResources::getCursorArrowDrag(Vector2I& hotSpot)
	{
		hotSpot = CursorArrowDragHotspot;
		return *mCursorArrowDrag.get();
	}

	const PixelData& BuiltinResources::getCursorWait(Vector2I& hotSpot)
	{
		hotSpot = CursorWaitHotspot;
		return *mCursorWait.get();
	}

	const PixelData& BuiltinResources::getCursorIBeam(Vector2I& hotSpot)
	{
		hotSpot = CursorIBeamHotspot;
		return *mCursorIBeam.get();
	}

	const PixelData& BuiltinResources::getCursorSizeNESW(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeNESWHotspot;
		return *mCursorSizeNESW.get();
	}

	const PixelData& BuiltinResources::getCursorSizeNS(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeNSHotspot;
		return *mCursorSizeNS.get();
	}

	const PixelData& BuiltinResources::getCursorSizeNWSE(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeNWSEHotspot;
		return *mCursorSizeNWSE.get();
	}

	const PixelData& BuiltinResources::getCursorSizeWE(Vector2I& hotSpot)
	{
		hotSpot = CursorSizeWEHotspot;
		return *mCursorSizeWE.get();
	}

	const PixelData& BuiltinResources::getCursorDeny(Vector2I& hotSpot)
	{
		hotSpot = CursorDenyHotspot;
		return *mCursorDeny.get();
	}

	const PixelData& BuiltinResources::getCursorMoveLeftRight(Vector2I& hotSpot)
	{
		hotSpot = CursorArrowLeftRightHotspot;
		return *mCursorArrowLeftRight.get();
	}

	GUIMaterialInfo BuiltinResources::createSpriteTextMaterial() const
	{
		GUIMaterialInfo info;
		info.material = Material::create(mShaderSpriteText);
		info.invViewportWidth = info.material->getParamFloat("invViewportWidth");
		info.invViewportHeight = info.material->getParamFloat("invViewportHeight");
		info.worldTransform = info.material->getParamMat4("worldTransform");
		info.mainTexture = info.material->getParamTexture("mainTexture");
		info.mainTexSampler = info.material->getParamSamplerState("mainTexSamp");
		info.tint = info.material->getParamVec4("tint");

		return info;
	}

	GUIMaterialInfo BuiltinResources::createSpriteImageMaterial() const
	{
		GUIMaterialInfo info;
		info.material = Material::create(mShaderSpriteImage);
		info.invViewportWidth = info.material->getParamFloat("invViewportWidth");
		info.invViewportHeight = info.material->getParamFloat("invViewportHeight");
		info.worldTransform = info.material->getParamMat4("worldTransform");
		info.mainTexture = info.material->getParamTexture("mainTexture");
		info.mainTexSampler = info.material->getParamSamplerState("mainTexSamp");
		info.tint = info.material->getParamVec4("tint");

		return info;
	}

	DebugDraw2DClipSpaceMatInfo BuiltinResources::createDebugDraw2DClipSpaceMaterial() const
	{
		DebugDraw2DClipSpaceMatInfo info;
		info.material = Material::create(mShaderDebugDraw2DClipSpace);

		return info;
	}

	DebugDraw2DScreenSpaceMatInfo BuiltinResources::createDebugDraw2DScreenSpaceMaterial() const
	{
		DebugDraw2DScreenSpaceMatInfo info;
		info.material = Material::create(mShaderDebugDraw2DScreenSpace);
		info.invViewportWidth = info.material->getParamFloat("invViewportWidth");
		info.invViewportHeight = info.material->getParamFloat("invViewportHeight");

		return info;
	}

	DebugDraw3DMatInfo BuiltinResources::createDebugDraw3DMaterial() const
	{
		DebugDraw3DMatInfo info;
		info.material = Material::create(mShaderDebugDraw3D);
		info.matViewProj = info.material->getParamMat4("matViewProj");

		return info;
	}

	HMaterial BuiltinResources::createDummyMaterial() const
	{
		return Material::create(mShaderDummy);
	}
}