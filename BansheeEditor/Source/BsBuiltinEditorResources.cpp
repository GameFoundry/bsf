#include "BsBuiltinEditorResources.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUIButton.h"
#include "BsGUIInputBox.h"
#include "BsGUIToggle.h"
#include "BsGUIColor.h"
#include "BsTextSprite.h"
#include "BsSpriteTexture.h"
#include "BsGUITreeViewEditBox.h"

#include "BsGUIIntField.h"
#include "BsGUIFloatField.h"
#include "BsGUIColorField.h"
#include "BsGUITextField.h"
#include "BsGUIToggleField.h"
#include "BsGUIVector2Field.h"
#include "BsGUIVector3Field.h"
#include "BsGUIVector4Field.h"
#include "BsGUIComponentFoldout.h"
#include "BsGUIFoldout.h"

#include "BsFont.h"
#include "BsFontImportOptions.h"
#include "BsImporter.h"
#include "BsGpuProgram.h"
#include "BsShader.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsMaterial.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"
#include "BsGpuProgramImportOptions.h"
#include "BsResources.h"
#include "BsRTTIType.h"
#include "BsFileSystem.h"

namespace BansheeEngine
{
	const String BuiltinEditorResources::ObjectFieldStyleName = "GUIObjectField";
	const String BuiltinEditorResources::ObjectFieldLabelStyleName = "EditorFieldLabel";
	const String BuiltinEditorResources::ObjectFieldDropBtnStyleName = "DropButton";
	const String BuiltinEditorResources::ObjectFieldClearBtnStyleName = "ObjectClearButton";

	const Path BuiltinEditorResources::DefaultSkinFolderRaw = L"..\\..\\..\\..\\Data\\Raw\\Editor\\Skin\\";
	const Path BuiltinEditorResources::DefaultShaderFolderRaw = L"..\\..\\..\\..\\Data\\Raw\\Editor\\Shaders\\";

	const Path BuiltinEditorResources::DefaultSkinFolder = L"..\\..\\..\\..\\Data\\Editor\\Skin\\";
	const Path BuiltinEditorResources::DefaultShaderFolder = L"..\\..\\..\\..\\Data\\Editor\\Shaders\\";
	
	const WString BuiltinEditorResources::HLSL11ShaderSubFolder = L"HLSL11/";
	const WString BuiltinEditorResources::HLSL9ShaderSubFolder = L"HLSL9/";
	const WString BuiltinEditorResources::GLSLShaderSubFolder = L"GLSL/";

	const WString BuiltinEditorResources::DefaultFontFilename = L"arial.ttf";
	const UINT32 BuiltinEditorResources::DefaultFontSize = 10;

	const WString BuiltinEditorResources::WindowBackgroundTexture = L"WindowBgTile.psd";

	const WString BuiltinEditorResources::ButtonNormalTex = L"ButtonNormal.psd";
	const WString BuiltinEditorResources::ButtonHoverTex = L"ButtonHover.psd";
	const WString BuiltinEditorResources::ButtonActiveTex = L"ButtonActive.psd";

	const WString BuiltinEditorResources::ToggleNormalTex = L"ToggleNormal.psd";
	const WString BuiltinEditorResources::ToggleHoverTex = L"ToggleHover.psd";
	const WString BuiltinEditorResources::ToggleActiveTex = L"ToggleActive.psd";
	const WString BuiltinEditorResources::ToggleNormalOnTex = L"ToggleOnNormal.psd";
	const WString BuiltinEditorResources::ToggleHoverOnTex = L"ToggleOnHover.psd";
	const WString BuiltinEditorResources::ToggleActiveOnTex = L"ToggleOnActive.psd";

	const WString BuiltinEditorResources::ObjectDropBtnNormalTex = L"ObjectFieldDropNormal.psd";
	const WString BuiltinEditorResources::ObjectDropBtnNormalOnTex = L"ObjectFieldDropOnNormal.psd";
	const WString BuiltinEditorResources::ObjectClearBtnNormalTex = L"ObjectFieldBtnNormal.psd";
	const WString BuiltinEditorResources::ObjectClearBtnHoverTex = L"ObjectFieldBtnHover.psd";
	const WString BuiltinEditorResources::ObjectClearBtnActiveTex = L"ObjectFieldBtnActive.psd";

	const WString BuiltinEditorResources::FoldoutOpenNormalTex = L"FoldoutOpenNormal.psd";
	const WString BuiltinEditorResources::FoldoutOpenHoverTex = L"FoldoutOpenHover.psd";
	const WString BuiltinEditorResources::FoldoutClosedNormalTex = L"FoldoutClosedNormal.psd";
	const WString BuiltinEditorResources::FoldoutClosedHoverTex = L"FoldoutClosedHover.psd";

	const WString BuiltinEditorResources::CmpFoldoutOpenNormalTex = L"CmpFoldoutOpenNormal.psd";
	const WString BuiltinEditorResources::CmpFoldoutOpenHoverTex = L"CmpFoldoutOpenHover.psd";
	const WString BuiltinEditorResources::CmpFoldoutOpenActiveTex = L"CmpFoldoutOpenActive.psd";
	const WString BuiltinEditorResources::CmpFoldoutClosedNormalTex = L"CmpFoldoutClosedNormal.psd";
	const WString BuiltinEditorResources::CmpFoldoutClosedHoverTex = L"CmpFoldoutClosedHover.psd";
	const WString BuiltinEditorResources::CmpFoldoutClosedActiveTex = L"CmpFoldoutClosedActive.psd";

	const WString BuiltinEditorResources::WindowFrameNormal = L"WindowFrameNormal.psd";
	const WString BuiltinEditorResources::WindowFrameFocused = L"WindowFrameFocused.psd";

	const WString BuiltinEditorResources::WindowTitleBarBg = L"WindowTitleBarBg.psd";

	const WString BuiltinEditorResources::WindowCloseButtonNormal = L"WindowCloseBtnNormal.psd";
	const WString BuiltinEditorResources::WindowCloseButtonHover = L"WindowCloseBtnHover.psd";

	const WString BuiltinEditorResources::WindowMinButtonNormal = L"WindowMaxBtnNormal.psd";
	const WString BuiltinEditorResources::WindowMinButtonHover = L"WindowMaxBtnHover.psd";

	const WString BuiltinEditorResources::WindowMaxButtonNormal = L"WindowMinBtnNormal.psd";
	const WString BuiltinEditorResources::WindowMaxButtonHover = L"WindowMinBtnHover.psd";

	const WString BuiltinEditorResources::TabbedBarBtnNormal = L"TabbedButtonNormal.psd";
	const WString BuiltinEditorResources::TabbedBarBtnActive = L"TabbedButtonActive.psd";

	const WString BuiltinEditorResources::InputBoxNormalTex = L"InputBoxNormal.psd";
	const WString BuiltinEditorResources::InputBoxHoverTex = L"InputBoxHover.psd";
	const WString BuiltinEditorResources::InputBoxFocusedTex = L"InputBoxFocused.psd";

	const WString BuiltinEditorResources::ScrollBarUpNormalTex = L"ScrollBarUpNormal.psd";
	const WString BuiltinEditorResources::ScrollBarUpHoverTex = L"ScrollBarUpHover.psd";
	const WString BuiltinEditorResources::ScrollBarUpActiveTex = L"ScrollBarUpActive.psd";

	const WString BuiltinEditorResources::ScrollBarDownNormalTex = L"ScrollBarDownNormal.psd";
	const WString BuiltinEditorResources::ScrollBarDownHoverTex = L"ScrollBarDownHover.psd";
	const WString BuiltinEditorResources::ScrollBarDownActiveTex = L"ScrollBarDownActive.psd";

	const WString BuiltinEditorResources::ScrollBarLeftNormalTex = L"ScrollBarLeftNormal.psd";
	const WString BuiltinEditorResources::ScrollBarLeftHoverTex = L"ScrollBarLeftHover.psd";
	const WString BuiltinEditorResources::ScrollBarLeftActiveTex = L"ScrollBarLeftActive.psd";

	const WString BuiltinEditorResources::ScrollBarRightNormalTex = L"ScrollBarRightNormal.psd";
	const WString BuiltinEditorResources::ScrollBarRightHoverTex = L"ScrollBarRightHover.psd";
	const WString BuiltinEditorResources::ScrollBarRightActiveTex = L"ScrollBarRightActive.psd";

	const WString BuiltinEditorResources::ScrollBarHandleHorzNormalTex = L"ScrollBarHorzHandleNormal.psd";
	const WString BuiltinEditorResources::ScrollBarHandleHorzHoverTex = L"ScrollBarHorzHandleHover.psd";
	const WString BuiltinEditorResources::ScrollBarHandleHorzActiveTex = L"ScrollBarHorzHandleActive.psd";

	const WString BuiltinEditorResources::ScrollBarHandleVertNormalTex = L"ScrollBarVertHandleNormal.psd";
	const WString BuiltinEditorResources::ScrollBarHandleVertHoverTex = L"ScrollBarVertHandleHover.psd";
	const WString BuiltinEditorResources::ScrollBarHandleVertActiveTex = L"ScrollBarVertHandleActive.psd";

	const WString BuiltinEditorResources::DropDownBtnNormalTex = L"DropDownNormal.psd";
	const WString BuiltinEditorResources::DropDownBtnHoverTex = L"DropDownHover.psd";

	const WString BuiltinEditorResources::DropDownBoxBgTex = L"DropDownBoxBg.psd";
	const WString BuiltinEditorResources::DropDownBoxEntryNormalTex = L"DropDownButtonNormal.psd";
	const WString BuiltinEditorResources::DropDownBoxEntryHoverTex = L"DropDownButtonHover.psd";

	const WString BuiltinEditorResources::DropDownBoxBtnUpNormalTex = L"DropDownBoxBtnUpNormal.psd";
	const WString BuiltinEditorResources::DropDownBoxBtnUpHoverTex = L"DropDownBoxBtnUpHover.psd";

	const WString BuiltinEditorResources::DropDownBoxBtnDownNormalTex = L"DropDownBoxBtnDownNormal.psd";
	const WString BuiltinEditorResources::DropDownBoxBtnDownHoverTex = L"DropDownBoxBtnDownHover.psd";

	const WString BuiltinEditorResources::DropDownBoxEntryExpNormalTex = L"DropDownExpNormal.psd";
	const WString BuiltinEditorResources::DropDownBoxEntryExpHoverTex = L"DropDownExpHover.psd";

	const WString BuiltinEditorResources::DropDownSeparatorTex = L"DropDownSeparator.psd";

	const WString BuiltinEditorResources::DropDownBoxBtnUpArrowTex = L"DropDownBoxBtnUpArrow.psd";
	const WString BuiltinEditorResources::DropDownBoxBtnDownArrowTex = L"DropDownBoxBtnDownArrow.psd";

	const WString BuiltinEditorResources::ScrollBarBgTex = L"ScrollBarBg.psd";

	const WString BuiltinEditorResources::MenuBarBgTex = L"MenuBarBg.psd";

	const WString BuiltinEditorResources::MenuBarBtnNormalTex = L"MenuBarButtonNormal.psd";
	const WString BuiltinEditorResources::MenuBarBtnHoverTex = L"MenuBarButtonHover.psd";

	const WString BuiltinEditorResources::MenuBarBansheeLogoTex = L"MenuBarBansheeLogo.psd";

	const WString BuiltinEditorResources::DockSliderNormalTex = L"DockSliderBtn.psd";

	const WString BuiltinEditorResources::TreeViewExpandButtonOffNormal = L"TreeViewExpandButtonOffNormal.psd";
	const WString BuiltinEditorResources::TreeViewExpandButtonOffHover = L"TreeViewExpandButtonOffHover.psd";
	const WString BuiltinEditorResources::TreeViewExpandButtonOnNormal = L"TreeViewExpandButtonOnNormal.psd";
	const WString BuiltinEditorResources::TreeViewExpandButtonOnHover = L"TreeViewExpandButtonOnHover.psd";

	const WString BuiltinEditorResources::TreeViewSelectionBackground = L"TreeViewSelectionBackground.psd";
	const WString BuiltinEditorResources::TreeViewEditBox = L"TreeViewEditBox.psd";
	const WString BuiltinEditorResources::TreeViewElementHighlight = L"TreeViewElementHighlight.psd";
	const WString BuiltinEditorResources::TreeViewElementSepHighlight = L"TreeViewElementSepHighlight.psd";

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

	const WString BuiltinEditorResources::ShaderDockOverlayVSFile = L"dockDropOverlayVS.gpuprog";
	const WString BuiltinEditorResources::ShaderDockOverlayPSFile = L"dockDropOverlayPS.gpuprog";
	const WString BuiltinEditorResources::SceneGridVSFile = L"sceneGridVS.gpuprog";
	const WString BuiltinEditorResources::SceneGridPSFile = L"sceneGridPS.gpuprog";
	const WString BuiltinEditorResources::PickingVSFile = L"pickingVS.gpuprog";
	const WString BuiltinEditorResources::PickingPSFile = L"pickingPS.gpuprog";
	const WString BuiltinEditorResources::PickingAlphaVSFile = L"pickingAlphaVS.gpuprog";
	const WString BuiltinEditorResources::PickingAlphaPSFile = L"pickingAlphaPS.gpuprog";
	const WString BuiltinEditorResources::ShaderWireGizmoVSFile = L"wireGizmoVS.gpuprog";
	const WString BuiltinEditorResources::ShaderWireGizmoPSFile = L"wireGizmoPS.gpuprog";
	const WString BuiltinEditorResources::ShaderSolidGizmoVSFile = L"solidGizmoVS.gpuprog";
	const WString BuiltinEditorResources::ShaderSolidGizmoPSFile = L"solidGizmoPS.gpuprog";
	const WString BuiltinEditorResources::ShaderIconGizmo0VSFile = L"iconGizmo0VS.gpuprog";
	const WString BuiltinEditorResources::ShaderIconGizmo0PSFile = L"iconGizmo0PS.gpuprog";
	const WString BuiltinEditorResources::ShaderIconGizmo1VSFile = L"iconGizmo1VS.gpuprog";
	const WString BuiltinEditorResources::ShaderIconGizmo1PSFile = L"iconGizmo1PS.gpuprog";
	const WString BuiltinEditorResources::GizmoPickingVSFile = L"pickingGizmoVS.gpuprog";
	const WString BuiltinEditorResources::GizmoPickingPSFile = L"pickingGizmoPS.gpuprog";
	const WString BuiltinEditorResources::GizmoPickingAlphaVSFile = L"pickingGizmoAlphaVS.gpuprog";
	const WString BuiltinEditorResources::GizmoPickingAlphaPSFile = L"pickingGizmoAlphaPS.gpuprog";

	BuiltinEditorResources::BuiltinEditorResources(RenderSystemPlugin activeRSPlugin)
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

		initDockDropOverlayShader();
		initSceneGridShader();
		initPickingShader(CULL_NONE);
		initPickingShader(CULL_CLOCKWISE);
		initPickingShader(CULL_COUNTERCLOCKWISE);
		initPickingAlphaShader(CULL_NONE);
		initPickingAlphaShader(CULL_CLOCKWISE);
		initPickingAlphaShader(CULL_COUNTERCLOCKWISE);
		initWireGizmoShader();
		initSolidGizmoShader();
		initIconGizmoShader();
		initGizmoPickingShader();
		initGizmoPickingAlphaShader();

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

		// Window frame
		GUIElementStyle windowFrameStyle;
		windowFrameStyle.normal.texture = getGUITexture(WindowFrameNormal);
		windowFrameStyle.focused.texture = getGUITexture(WindowFrameFocused);
		windowFrameStyle.border.left = 1;
		windowFrameStyle.border.right = 1;
		windowFrameStyle.border.top = 1;
		windowFrameStyle.border.bottom = 1;

		mSkin.setStyle("WindowFrame", windowFrameStyle);

		// Button
		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = getGUITexture(ButtonNormalTex);
		buttonStyle.hover.texture = getGUITexture(ButtonHoverTex);
		buttonStyle.active.texture = getGUITexture(ButtonActiveTex);
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
		toggleStyle.normal.texture = getGUITexture(ToggleNormalTex);
		toggleStyle.hover.texture = getGUITexture(ToggleHoverTex);
		toggleStyle.active.texture = getGUITexture(ToggleActiveTex);
		toggleStyle.normalOn.texture = getGUITexture(ToggleNormalOnTex);
		toggleStyle.hoverOn.texture = getGUITexture(ToggleHoverOnTex);
		toggleStyle.activeOn.texture = getGUITexture(ToggleActiveOnTex);
		toggleStyle.fixedHeight = true;
		toggleStyle.fixedWidth = true;
		toggleStyle.height = 15;
		toggleStyle.width = 15;

		mSkin.setStyle(GUIToggle::getGUITypeName(), toggleStyle);

		// Color
		GUIElementStyle colorStyle;
		colorStyle.margins.left = 2;
		colorStyle.margins.right = 2;
		colorStyle.margins.top = 2;
		colorStyle.margins.bottom = 2;
		colorStyle.fixedHeight = true;
		colorStyle.height = 10;
		colorStyle.minWidth = 10;

		mSkin.setStyle(GUIColor::getGUITypeName(), colorStyle);

		// Window background texture
		GUIElementStyle windowBgStyle;
		windowBgStyle.normal.texture = getGUITexture(WindowBackgroundTexture);

		mSkin.setStyle("WindowBackground", windowBgStyle);

		// Window title bar background
		GUIElementStyle titleBarBgStyle;
		titleBarBgStyle.normal.texture = getGUITexture(WindowTitleBarBg);
		titleBarBgStyle.fixedHeight = true;
		titleBarBgStyle.height = 13;

		mSkin.setStyle("TitleBarBackground", titleBarBgStyle);

		// Tabbed title bar tab button
		GUIElementStyle tabbedBarButton;
		tabbedBarButton.normal.texture = getGUITexture(TabbedBarBtnNormal);
		tabbedBarButton.hover.texture = getGUITexture(TabbedBarBtnActive);
		tabbedBarButton.active.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.normalOn.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.hoverOn.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.activeOn.texture = tabbedBarButton.hover.texture;
		tabbedBarButton.fixedHeight = true;
		tabbedBarButton.height = 13;
		tabbedBarButton.minWidth = 10;
		tabbedBarButton.maxWidth = 110;
		tabbedBarButton.font = font;
		tabbedBarButton.fontSize = DefaultFontSize;
		tabbedBarButton.textHorzAlign = THA_Center;
		tabbedBarButton.textVertAlign = TVA_Center;

		mSkin.setStyle("TabbedBarBtn", tabbedBarButton);

		// Tabbed title bar drag/drop button
		GUIElementStyle tabbedBarDropButton;
		tabbedBarDropButton.fixedHeight = true;
		tabbedBarDropButton.fixedWidth = true;
		tabbedBarDropButton.height = 13;
		tabbedBarDropButton.width = 6;

		mSkin.setStyle("TabbedBarDropArea", tabbedBarDropButton);

		// Window minimize button
		GUIElementStyle winMinButtonStyle;
		winMinButtonStyle.normal.texture = getGUITexture(WindowMinButtonNormal);
		winMinButtonStyle.hover.texture = getGUITexture(WindowMinButtonHover);
		winMinButtonStyle.active.texture = winMinButtonStyle.hover.texture;
		winMinButtonStyle.fixedHeight = true;
		winMinButtonStyle.fixedWidth = true;
		winMinButtonStyle.height = 7;
		winMinButtonStyle.width = 8;

		mSkin.setStyle("WinMinimizeBtn", winMinButtonStyle);

		// Window maximize button
		GUIElementStyle winMaxButtonStyle;
		winMaxButtonStyle.normal.texture = getGUITexture(WindowMaxButtonNormal);
		winMaxButtonStyle.hover.texture = getGUITexture(WindowMaxButtonHover);
		winMaxButtonStyle.active.texture = winMaxButtonStyle.hover.texture;
		winMaxButtonStyle.fixedHeight = true;
		winMaxButtonStyle.fixedWidth = true;
		winMaxButtonStyle.height = 8;
		winMaxButtonStyle.width = 8;

		mSkin.setStyle("WinMaximizeBtn", winMaxButtonStyle);

		// Window close button
		GUIElementStyle winCloseButtonStyle;
		winCloseButtonStyle.normal.texture = getGUITexture(WindowCloseButtonNormal);
		winCloseButtonStyle.hover.texture = getGUITexture(WindowCloseButtonHover);
		winCloseButtonStyle.active.texture = winCloseButtonStyle.hover.texture;
		winCloseButtonStyle.fixedHeight = true;
		winCloseButtonStyle.fixedWidth = true;
		winCloseButtonStyle.height = 7;
		winCloseButtonStyle.width = 8;

		mSkin.setStyle("WinCloseBtn", winCloseButtonStyle);

		// Input box
		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = getGUITexture(InputBoxNormalTex);
		inputBoxStyle.hover.texture = getGUITexture(InputBoxHoverTex);
		inputBoxStyle.focused.texture = getGUITexture(InputBoxFocusedTex);
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
		scrollUpBtnStyle.normal.texture = getGUITexture(ScrollBarUpNormalTex);
		scrollUpBtnStyle.hover.texture = getGUITexture(ScrollBarUpHoverTex);
		scrollUpBtnStyle.active.texture = getGUITexture(ScrollBarUpActiveTex);
		scrollUpBtnStyle.fixedHeight = true;
		scrollUpBtnStyle.fixedWidth = true;
		scrollUpBtnStyle.height = 4;
		scrollUpBtnStyle.width = 8;

		mSkin.setStyle("ScrollUpBtn", scrollUpBtnStyle);

		// Down button
		GUIElementStyle scrollDownBtnStyle;
		scrollDownBtnStyle.normal.texture = getGUITexture(ScrollBarDownNormalTex);
		scrollDownBtnStyle.hover.texture = getGUITexture(ScrollBarDownHoverTex);
		scrollDownBtnStyle.active.texture = getGUITexture(ScrollBarDownActiveTex);
		scrollDownBtnStyle.fixedHeight = true;
		scrollDownBtnStyle.fixedWidth = true;
		scrollDownBtnStyle.height = 4;
		scrollDownBtnStyle.width = 8;

		mSkin.setStyle("ScrollDownBtn", scrollDownBtnStyle);

		// Left button
		GUIElementStyle scrollLeftBtnStyle;
		scrollLeftBtnStyle.normal.texture = getGUITexture(ScrollBarLeftNormalTex);
		scrollLeftBtnStyle.hover.texture = getGUITexture(ScrollBarLeftHoverTex);
		scrollLeftBtnStyle.active.texture = getGUITexture(ScrollBarLeftActiveTex);
		scrollLeftBtnStyle.fixedHeight = true;
		scrollLeftBtnStyle.fixedWidth = true;
		scrollLeftBtnStyle.height = 8;
		scrollLeftBtnStyle.width = 4;

		mSkin.setStyle("ScrollLeftBtn", scrollLeftBtnStyle);

		// Right button
		GUIElementStyle scrollRightBtnStyle;
		scrollRightBtnStyle.normal.texture = getGUITexture(ScrollBarRightNormalTex);
		scrollRightBtnStyle.hover.texture = getGUITexture(ScrollBarRightHoverTex);
		scrollRightBtnStyle.active.texture = getGUITexture(ScrollBarRightActiveTex);
		scrollRightBtnStyle.fixedHeight = true;
		scrollRightBtnStyle.fixedWidth = true;
		scrollRightBtnStyle.height = 8;
		scrollRightBtnStyle.width = 4;

		mSkin.setStyle("ScrollRightBtn", scrollRightBtnStyle);

		// Horizontal handle
		GUIElementStyle scrollBarHorzBtnStyle;
		scrollBarHorzBtnStyle.normal.texture = getGUITexture(ScrollBarHandleHorzNormalTex);
		scrollBarHorzBtnStyle.hover.texture = getGUITexture(ScrollBarHandleHorzHoverTex);
		scrollBarHorzBtnStyle.active.texture = getGUITexture(ScrollBarHandleHorzActiveTex);
		scrollBarHorzBtnStyle.fixedHeight = true;
		scrollBarHorzBtnStyle.fixedWidth = true;
		scrollBarHorzBtnStyle.height = 6;
		scrollBarHorzBtnStyle.width = 4;

		mSkin.setStyle("ScrollBarHorzBtn", scrollBarHorzBtnStyle);

		// Vertical handle
		GUIElementStyle scrollBarVertBtnStyle;
		scrollBarVertBtnStyle.normal.texture = getGUITexture(ScrollBarHandleVertNormalTex);
		scrollBarVertBtnStyle.hover.texture = getGUITexture(ScrollBarHandleVertHoverTex);
		scrollBarVertBtnStyle.active.texture = getGUITexture(ScrollBarHandleVertActiveTex);
		scrollBarVertBtnStyle.fixedHeight = true;
		scrollBarVertBtnStyle.fixedWidth = true;
		scrollBarVertBtnStyle.height = 4;
		scrollBarVertBtnStyle.width = 6;

		mSkin.setStyle("ScrollBarVertBtn", scrollBarVertBtnStyle);

		HSpriteTexture scrollBarBgPtr = getGUITexture(ScrollBarBgTex);

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
		dropDownListStyle.normal.texture = getGUITexture(DropDownBtnNormalTex);
		dropDownListStyle.hover.texture = getGUITexture(DropDownBtnHoverTex);
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
		HSpriteTexture dropDownBtnScrollUpArrow = getGUITexture(DropDownBoxBtnUpArrowTex);

		GUIElementStyle dropDownScrollUpBtnArrowStyle;
		dropDownScrollUpBtnArrowStyle.normal.texture = getGUITexture(DropDownBoxBtnUpArrowTex);
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
		dropDownScrollUpBtnStyle.normal.texture = getGUITexture(DropDownBoxBtnUpNormalTex);
		dropDownScrollUpBtnStyle.hover.texture = getGUITexture(DropDownBoxBtnUpHoverTex);
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
		dropDownScrollDownBtnArrowStyle.normal.texture = getGUITexture(DropDownBoxBtnDownArrowTex);
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
		dropDownScrollDownBtnStyle.normal.texture = getGUITexture(DropDownBoxBtnDownNormalTex);
		dropDownScrollDownBtnStyle.hover.texture = getGUITexture(DropDownBoxBtnDownHoverTex);
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
		dropDownEntryBtnStyle.normal.texture = getGUITexture(DropDownBoxEntryNormalTex);
		dropDownEntryBtnStyle.hover.texture = getGUITexture(DropDownBoxEntryHoverTex);
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
		dropDownEntryExpBtnStyle.normal.texture = getGUITexture(DropDownBoxEntryExpNormalTex);
		dropDownEntryExpBtnStyle.hover.texture = getGUITexture(DropDownBoxEntryExpHoverTex);
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
		dropDownBoxStyle.normal.texture = getGUITexture(DropDownBoxBgTex);
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
		dropDownSeparatorStyle.normal.texture = getGUITexture(DropDownSeparatorTex);
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

		/************************************************************************/
		/* 								MENU BAR	                     		*/
		/************************************************************************/

		// MenuBar background
		GUIElementStyle menuBarBgStyle;
		menuBarBgStyle.normal.texture = getGUITexture(MenuBarBgTex);
		menuBarBgStyle.fixedHeight = false;
		menuBarBgStyle.fixedWidth = false;
		menuBarBgStyle.height = 4;
		menuBarBgStyle.width = 4;

		mSkin.setStyle("MenuBarBg", menuBarBgStyle);

		// MenuBar Banshee logo
		GUIElementStyle menuBarBansheeLogoStyle;
		menuBarBansheeLogoStyle.normal.texture = getGUITexture(MenuBarBansheeLogoTex);
		menuBarBansheeLogoStyle.fixedHeight = true;
		menuBarBansheeLogoStyle.fixedWidth = true;
		menuBarBansheeLogoStyle.height = 7;
		menuBarBansheeLogoStyle.width = 51;

		mSkin.setStyle("MenuBarBansheeLogo", menuBarBansheeLogoStyle);

		// MenuBar button
		GUIElementStyle menuBarBtnStyle;
		menuBarBtnStyle.normal.texture = getGUITexture(MenuBarBtnNormalTex);
		menuBarBtnStyle.hover.texture = getGUITexture(MenuBarBtnHoverTex);
		menuBarBtnStyle.active.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.normalOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.hoverOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.activeOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.fixedHeight = true;
		menuBarBtnStyle.fixedWidth = false;
		menuBarBtnStyle.height = 15;
		menuBarBtnStyle.width = 4;
		menuBarBtnStyle.margins.left = 2;
		menuBarBtnStyle.margins.right = 2;
		menuBarBtnStyle.margins.top = 2;
		menuBarBtnStyle.margins.bottom = 2;
		menuBarBtnStyle.font = font;
		menuBarBtnStyle.fontSize = DefaultFontSize;
		menuBarBtnStyle.textHorzAlign = THA_Left;
		menuBarBtnStyle.textVertAlign = TVA_Top;

		mSkin.setStyle("MenuBarBtn", menuBarBtnStyle);

		/************************************************************************/
		/* 								DOCK SLIDER	                     		*/
		/************************************************************************/

		GUIElementStyle dockSliderBtnStyle;
		dockSliderBtnStyle.normal.texture = getGUITexture(DockSliderNormalTex);
		dockSliderBtnStyle.fixedHeight = false;
		dockSliderBtnStyle.fixedWidth = false;
		dockSliderBtnStyle.height = 2;
		dockSliderBtnStyle.width = 2;

		mSkin.setStyle("DockSliderBtn", dockSliderBtnStyle);

		/************************************************************************/
		/* 								TREE VIEW	                     		*/
		/************************************************************************/

		// Expand button
		GUIElementStyle treeViewExpandButtonStyle;
		treeViewExpandButtonStyle.normal.texture = getGUITexture(TreeViewExpandButtonOffNormal);
		treeViewExpandButtonStyle.hover.texture = getGUITexture(TreeViewExpandButtonOffHover);
		treeViewExpandButtonStyle.active.texture = treeViewExpandButtonStyle.hover.texture;
		treeViewExpandButtonStyle.normalOn.texture = getGUITexture(TreeViewExpandButtonOnNormal);
		treeViewExpandButtonStyle.hoverOn.texture = getGUITexture(TreeViewExpandButtonOnHover);
		treeViewExpandButtonStyle.activeOn.texture = treeViewExpandButtonStyle.hoverOn.texture;
		treeViewExpandButtonStyle.margins.left = 4;
		treeViewExpandButtonStyle.margins.right = 4;
		treeViewExpandButtonStyle.margins.top = 5;
		treeViewExpandButtonStyle.margins.bottom = 4;
		treeViewExpandButtonStyle.fixedHeight = true;
		treeViewExpandButtonStyle.fixedWidth = true;
		treeViewExpandButtonStyle.height = 16;
		treeViewExpandButtonStyle.width = 16;

		mSkin.setStyle("TreeViewFoldoutBtn", treeViewExpandButtonStyle);

		// Entry
		GUIElementStyle treeViewEntryStyle;
		treeViewEntryStyle.font = font;
		treeViewEntryStyle.fontSize = DefaultFontSize;
		treeViewEntryStyle.fixedWidth = false;
		treeViewEntryStyle.fixedHeight = true;
		treeViewEntryStyle.height = 16;
		treeViewEntryStyle.minWidth = 10;

		mSkin.setStyle("TreeViewElementBtn", treeViewEntryStyle);

		// Selection background
		GUIElementStyle treeViewSelBackgroundStyle;
		treeViewSelBackgroundStyle.normal.texture = getGUITexture(TreeViewSelectionBackground);
		treeViewSelBackgroundStyle.fixedHeight = false;
		treeViewSelBackgroundStyle.fixedWidth = false;
		treeViewSelBackgroundStyle.height = 2;
		treeViewSelBackgroundStyle.width = 2;

		mSkin.setStyle("TreeViewSelectionBackground", treeViewSelBackgroundStyle);

		// Edit box
		GUIElementStyle treeViewEditBox;
		treeViewEditBox.normal.texture = getGUITexture(TreeViewEditBox);
		treeViewEditBox.hover.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.focused.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.active.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.border.left = 1;
		treeViewEditBox.border.right = 1;
		treeViewEditBox.border.top = 1;
		treeViewEditBox.border.bottom = 1;
		treeViewEditBox.margins.left = 1;
		treeViewEditBox.margins.right = 1;
		treeViewEditBox.margins.top = 1;
		treeViewEditBox.margins.bottom = 1;
		treeViewEditBox.fixedHeight = true;
		treeViewEditBox.height = 13;
		treeViewEditBox.minWidth = 10;
		treeViewEditBox.font = font;
		treeViewEditBox.fontSize = DefaultFontSize;
		treeViewEditBox.textHorzAlign = THA_Left;
		treeViewEditBox.textVertAlign = TVA_Top;

		mSkin.setStyle(GUITreeViewEditBox::getGUITypeName(), treeViewEditBox);

		// Element highlight
		GUIElementStyle treeViewElementHighlight;
		treeViewElementHighlight.normal.texture = getGUITexture(TreeViewElementHighlight);
		treeViewElementHighlight.border.left = 1;
		treeViewElementHighlight.border.right = 1;
		treeViewElementHighlight.border.top = 1;
		treeViewElementHighlight.border.bottom = 1;

		mSkin.setStyle("TreeViewElementHighlight", treeViewElementHighlight);

		// Element separator highlight
		GUIElementStyle treeViewElementSepHighlight;
		treeViewElementSepHighlight.normal.texture = getGUITexture(TreeViewElementSepHighlight);
		treeViewElementSepHighlight.border.left = 1;
		treeViewElementSepHighlight.border.right = 1;
		treeViewElementSepHighlight.border.top = 1;
		treeViewElementSepHighlight.border.bottom = 1;

		mSkin.setStyle("TreeViewElementSepHighlight", treeViewElementSepHighlight);
	
		/************************************************************************/
		/* 							OBJECT DROP FIELD                      		*/
		/************************************************************************/
		GUIElementStyle objectDropStyle;
		objectDropStyle.normal.texture = getGUITexture(ObjectDropBtnNormalTex);
		objectDropStyle.normalOn.texture = getGUITexture(ObjectDropBtnNormalOnTex);
		objectDropStyle.fixedHeight = true;
		objectDropStyle.height = 15;
		objectDropStyle.minWidth = 50;
		objectDropStyle.font = font;
		objectDropStyle.fontSize = DefaultFontSize;
		objectDropStyle.textHorzAlign = THA_Center;
		objectDropStyle.textVertAlign = TVA_Center;

		mSkin.setStyle(ObjectFieldDropBtnStyleName, objectDropStyle);

		GUIElementStyle objectClearBtnStyle;
		objectClearBtnStyle.normal.texture = getGUITexture(ObjectClearBtnNormalTex);
		objectClearBtnStyle.hover.texture = getGUITexture(ObjectClearBtnHoverTex);
		objectClearBtnStyle.active.texture = getGUITexture(ObjectClearBtnActiveTex);
		objectClearBtnStyle.fixedHeight = true;
		objectClearBtnStyle.fixedWidth = true;
		objectClearBtnStyle.height = 15;
		objectClearBtnStyle.width = 13;

		mSkin.setStyle(ObjectFieldClearBtnStyleName, objectClearBtnStyle);

		GUIElementStyle editorObjectFieldStyle;
		editorObjectFieldStyle.fixedHeight = true;
		editorObjectFieldStyle.height = 15;
		editorObjectFieldStyle.minWidth = 30;
		editorObjectFieldStyle.subStyles[ObjectFieldLabelStyleName] = GUITextField::getLabelStyleType();
		editorObjectFieldStyle.subStyles[ObjectFieldDropBtnStyleName] = ObjectFieldDropBtnStyleName;
		editorObjectFieldStyle.subStyles[ObjectFieldClearBtnStyleName] = ObjectFieldClearBtnStyleName;

		mSkin.setStyle(ObjectFieldStyleName, editorObjectFieldStyle);

		/************************************************************************/
		/* 								EDITOR FIELDS                      		*/
		/************************************************************************/

		GUIElementStyle editorFieldLabelStyle;
		editorFieldLabelStyle.font = font;
		editorFieldLabelStyle.fontSize = DefaultFontSize;
		editorFieldLabelStyle.fixedWidth = false;
		editorFieldLabelStyle.fixedHeight = true;
		editorFieldLabelStyle.height = 11;
		editorFieldLabelStyle.minWidth = 10;
		editorFieldLabelStyle.textHorzAlign = THA_Left;

		mSkin.setStyle(GUITextField::getLabelStyleType(), editorFieldLabelStyle);

		GUIElementStyle editorIntFieldStyle;
		editorIntFieldStyle.fixedHeight = true;
		editorIntFieldStyle.height = 15;
		editorIntFieldStyle.minWidth = 30;
		editorIntFieldStyle.subStyles[GUIIntField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorIntFieldStyle.subStyles[GUIIntField::getInputStyleType()] = GUIInputBox::getGUITypeName();

		mSkin.setStyle(GUIIntField::getGUITypeName(), editorIntFieldStyle);

		GUIElementStyle editorFloatFieldStyle;
		editorFloatFieldStyle.fixedHeight = true;
		editorFloatFieldStyle.height = 15;
		editorFloatFieldStyle.minWidth = 30;
		editorFloatFieldStyle.subStyles[GUIFloatField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorFloatFieldStyle.subStyles[GUIFloatField::getInputStyleType()] = GUIInputBox::getGUITypeName();

		mSkin.setStyle(GUIFloatField::getGUITypeName(), editorFloatFieldStyle);

		GUIElementStyle editorTextFieldStyle;
		editorTextFieldStyle.fixedHeight = true;
		editorTextFieldStyle.height = 15;
		editorTextFieldStyle.minWidth = 30;
		editorTextFieldStyle.subStyles[GUITextField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorTextFieldStyle.subStyles[GUITextField::getInputStyleType()] = GUIInputBox::getGUITypeName();

		mSkin.setStyle(GUITextField::getGUITypeName(), editorTextFieldStyle);

		GUIElementStyle editorColorFieldStyle;
		editorColorFieldStyle.fixedHeight = true;
		editorColorFieldStyle.height = 15;
		editorColorFieldStyle.minWidth = 30;
		editorColorFieldStyle.subStyles[GUIColorField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorColorFieldStyle.subStyles[GUIColorField::getColorInputStyleType()] = GUIColor::getGUITypeName();

		mSkin.setStyle(GUIColorField::getGUITypeName(), editorColorFieldStyle);

		GUIElementStyle editorToggleFieldStyle;
		editorToggleFieldStyle.fixedHeight = true;
		editorToggleFieldStyle.height = 15;
		editorToggleFieldStyle.minWidth = 30;
		editorToggleFieldStyle.subStyles[GUIToggleField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorToggleFieldStyle.subStyles[GUIToggleField::getToggleStyleType()] = GUIToggle::getGUITypeName();

		mSkin.setStyle(GUIToggleField::getGUITypeName(), editorToggleFieldStyle);

		GUIElementStyle editorVector2FieldStyle;
		editorVector2FieldStyle.fixedHeight = true;
		editorVector2FieldStyle.height = 30;
		editorVector2FieldStyle.minWidth = 30;
		editorVector2FieldStyle.subStyles[GUIVector2Field::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorVector2FieldStyle.subStyles[GUIVector2Field::getFloatFieldStyleType()] = GUIFloatField::getGUITypeName();

		mSkin.setStyle(GUIVector2Field::getGUITypeName(), editorVector2FieldStyle);

		GUIElementStyle editorVector3FieldStyle;
		editorVector3FieldStyle.fixedHeight = true;
		editorVector3FieldStyle.height = 30;
		editorVector3FieldStyle.minWidth = 30;
		editorVector3FieldStyle.subStyles[GUIVector3Field::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorVector3FieldStyle.subStyles[GUIVector3Field::getFloatFieldStyleType()] = GUIFloatField::getGUITypeName();

		mSkin.setStyle(GUIVector3Field::getGUITypeName(), editorVector3FieldStyle);

		GUIElementStyle editorVector4FieldStyle;
		editorVector4FieldStyle.fixedHeight = true;
		editorVector4FieldStyle.height = 30;
		editorVector4FieldStyle.minWidth = 30;
		editorVector4FieldStyle.subStyles[GUIVector4Field::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorVector4FieldStyle.subStyles[GUIVector4Field::getFloatFieldStyleType()] = GUIFloatField::getGUITypeName();

		mSkin.setStyle(GUIVector4Field::getGUITypeName(), editorVector4FieldStyle);

		/************************************************************************/
		/* 							COMPONENT FOLDOUT                      		*/
		/************************************************************************/
		GUIElementStyle cmpFoldoutBtnStyle;
		cmpFoldoutBtnStyle.normal.texture = getGUITexture(CmpFoldoutClosedNormalTex);
		cmpFoldoutBtnStyle.hover.texture = getGUITexture(CmpFoldoutClosedHoverTex);
		cmpFoldoutBtnStyle.active.texture = getGUITexture(CmpFoldoutOpenActiveTex);
		cmpFoldoutBtnStyle.normalOn.texture = getGUITexture(CmpFoldoutOpenNormalTex);
		cmpFoldoutBtnStyle.hoverOn.texture = getGUITexture(CmpFoldoutOpenHoverTex);
		cmpFoldoutBtnStyle.activeOn.texture = getGUITexture(CmpFoldoutOpenActiveTex);
		cmpFoldoutBtnStyle.fixedHeight = true;
		cmpFoldoutBtnStyle.fixedWidth = false;
		cmpFoldoutBtnStyle.height = 10;
		cmpFoldoutBtnStyle.minWidth = 10;
		cmpFoldoutBtnStyle.font = font;
		cmpFoldoutBtnStyle.fontSize = DefaultFontSize;
		cmpFoldoutBtnStyle.textHorzAlign = THA_Left;
		cmpFoldoutBtnStyle.textVertAlign = TVA_Center;
		cmpFoldoutBtnStyle.contentOffset = RectOffset(12, 0, 0, 0);
		cmpFoldoutBtnStyle.border.left = 8;

		mSkin.setStyle(GUIComponentFoldout::getFoldoutButtonStyleType(), cmpFoldoutBtnStyle);

		GUIElementStyle cmpFoldoutStyle;
		cmpFoldoutStyle.fixedHeight = true;
		cmpFoldoutStyle.height = 12;
		cmpFoldoutStyle.minWidth = 30;
		cmpFoldoutStyle.subStyles[GUIComponentFoldout::getFoldoutButtonStyleType()] = GUIComponentFoldout::getFoldoutButtonStyleType();

		mSkin.setStyle(GUIComponentFoldout::getGUITypeName(), cmpFoldoutStyle);

		/************************************************************************/
		/* 							     FOLDOUT                      		    */
		/************************************************************************/
		GUIElementStyle foldoutBtnStyle;
		foldoutBtnStyle.normal.texture = getGUITexture(FoldoutClosedNormalTex);
		foldoutBtnStyle.hover.texture = getGUITexture(FoldoutClosedHoverTex);
		foldoutBtnStyle.active.texture = foldoutBtnStyle.hover.texture;
		foldoutBtnStyle.normalOn.texture = getGUITexture(FoldoutOpenNormalTex);
		foldoutBtnStyle.hoverOn.texture = getGUITexture(FoldoutOpenHoverTex);
		foldoutBtnStyle.activeOn.texture = foldoutBtnStyle.hoverOn.texture;
		foldoutBtnStyle.fixedHeight = true;
		foldoutBtnStyle.fixedWidth = true;
		foldoutBtnStyle.height = 10;
		foldoutBtnStyle.width = 8;

		mSkin.setStyle(GUIFoldout::getFoldoutButtonStyleType(), foldoutBtnStyle);

		GUIElementStyle foldoutStyle;
		foldoutStyle.fixedHeight = true;
		foldoutStyle.height = 12;
		foldoutStyle.minWidth = 30;
		foldoutStyle.subStyles[GUIFoldout::getLabelStyleType()] = GUIFoldout::getLabelStyleType();
		foldoutStyle.subStyles[GUIFoldout::getFoldoutButtonStyleType()] = GUIFoldout::getFoldoutButtonStyleType();

		mSkin.setStyle(GUIFoldout::getGUITypeName(), foldoutStyle);
	}

	void BuiltinEditorResources::preprocess()
	{
		static const WString GUI_TEXTURES[] = 
		{   WindowBackgroundTexture, ButtonNormalTex, ButtonHoverTex, ButtonActiveTex, ToggleNormalTex,
			ToggleHoverTex, ToggleActiveTex, ToggleNormalOnTex, ToggleHoverOnTex, ToggleActiveOnTex,
			ObjectDropBtnNormalTex, ObjectDropBtnNormalOnTex, ObjectClearBtnNormalTex, ObjectClearBtnHoverTex,
			ObjectClearBtnActiveTex, FoldoutOpenNormalTex, FoldoutOpenHoverTex, FoldoutClosedNormalTex,
			FoldoutClosedHoverTex, CmpFoldoutOpenNormalTex, CmpFoldoutOpenHoverTex, CmpFoldoutOpenActiveTex,
			CmpFoldoutClosedNormalTex, CmpFoldoutClosedHoverTex, CmpFoldoutClosedActiveTex, WindowFrameNormal,
			WindowFrameFocused, WindowTitleBarBg, WindowCloseButtonNormal, WindowCloseButtonHover, WindowMinButtonNormal,
			WindowMinButtonHover, WindowMaxButtonNormal, WindowMaxButtonHover, TabbedBarBtnNormal, TabbedBarBtnActive,
			InputBoxNormalTex, InputBoxHoverTex, InputBoxFocusedTex, ScrollBarUpNormalTex, ScrollBarUpHoverTex,
			ScrollBarUpActiveTex, ScrollBarDownNormalTex, ScrollBarDownHoverTex, ScrollBarDownActiveTex, ScrollBarLeftNormalTex,
			ScrollBarLeftHoverTex, ScrollBarLeftActiveTex, ScrollBarRightNormalTex, ScrollBarRightHoverTex, ScrollBarRightActiveTex,
			ScrollBarHandleHorzNormalTex, ScrollBarHandleHorzHoverTex, ScrollBarHandleHorzActiveTex, ScrollBarHandleVertNormalTex,
			ScrollBarHandleVertHoverTex, ScrollBarHandleVertActiveTex, DropDownBtnNormalTex, DropDownBtnHoverTex,
			DropDownBoxBgTex, DropDownBoxEntryNormalTex, DropDownBoxEntryHoverTex, DropDownBoxBtnUpNormalTex,
			DropDownBoxBtnUpHoverTex, DropDownBoxBtnDownNormalTex, DropDownBoxBtnDownHoverTex, DropDownBoxEntryExpNormalTex,
			DropDownBoxEntryExpHoverTex, DropDownSeparatorTex, DropDownBoxBtnUpArrowTex, DropDownBoxBtnDownArrowTex,
			ScrollBarBgTex, MenuBarBgTex, MenuBarBtnNormalTex, MenuBarBtnHoverTex, MenuBarBansheeLogoTex, DockSliderNormalTex,
			TreeViewExpandButtonOffNormal, TreeViewExpandButtonOffHover, TreeViewExpandButtonOnNormal, TreeViewExpandButtonOnHover,
			TreeViewSelectionBackground, TreeViewEditBox, TreeViewElementHighlight, TreeViewElementSepHighlight };

		static const GpuProgramImportData GPU_PROGRAM_IMPORT_DATA[] = 
		{
			{ SceneGridVSFile,			"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ SceneGridPSFile,			"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderDockOverlayVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderDockOverlayPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ PickingVSFile,			"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ PickingPSFile,			"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ PickingAlphaVSFile,		"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ PickingAlphaPSFile,		"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderWireGizmoVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderWireGizmoPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderSolidGizmoVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderSolidGizmoPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderIconGizmo0VSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderIconGizmo0PSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderIconGizmo1VSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ ShaderIconGizmo1PSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ GizmoPickingVSFile,		"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ GizmoPickingPSFile,		"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ GizmoPickingAlphaVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ GizmoPickingAlphaPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"hlsl", HLSL11ShaderSubFolder },
			{ SceneGridVSFile,			"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_2_0,		"hlsl", HLSL9ShaderSubFolder },
			{ SceneGridPSFile,			"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_2_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderDockOverlayVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_2_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderDockOverlayPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_2_0,		"hlsl", HLSL9ShaderSubFolder },
			{ PickingVSFile,			"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ PickingPSFile,			"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ PickingAlphaVSFile,		"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ PickingAlphaPSFile,		"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderWireGizmoVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderWireGizmoPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderSolidGizmoVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderSolidGizmoPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderIconGizmo0VSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderIconGizmo0PSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderIconGizmo1VSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ ShaderIconGizmo1PSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ GizmoPickingVSFile,		"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ GizmoPickingPSFile,		"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ GizmoPickingAlphaVSFile,	"vs_main",	GPT_VERTEX_PROGRAM,		GPP_VS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ GizmoPickingAlphaPSFile,	"ps_main",	GPT_FRAGMENT_PROGRAM,	GPP_FS_3_0,		"hlsl", HLSL9ShaderSubFolder },
			{ SceneGridVSFile,			"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ SceneGridPSFile,			"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderDockOverlayVSFile,	"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderDockOverlayPSFile,	"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ PickingVSFile,			"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ PickingPSFile,			"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ PickingAlphaVSFile,		"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ PickingAlphaPSFile,		"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderWireGizmoVSFile,	"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderWireGizmoPSFile,	"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderSolidGizmoVSFile,	"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderSolidGizmoPSFile,	"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderIconGizmo0VSFile,	"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderIconGizmo0PSFile,	"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderIconGizmo1VSFile,	"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ ShaderIconGizmo1PSFile,	"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ GizmoPickingVSFile,		"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ GizmoPickingPSFile,		"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
			{ GizmoPickingAlphaVSFile,	"main",		GPT_VERTEX_PROGRAM,		GPP_VS_4_0,		"glsl", GLSLShaderSubFolder },
			{ GizmoPickingAlphaPSFile,	"main",		GPT_FRAGMENT_PROGRAM,	GPP_FS_4_0,		"glsl", GLSLShaderSubFolder },
		};

		if (FileSystem::exists(DefaultSkinFolderRaw))
		{
			FileSystem::remove(DefaultSkinFolder);

			for (auto& tex : GUI_TEXTURES)
				importGUITexture(tex);

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

				HFont font = Importer::instance().import<Font>(fontPath, fontImportOptions);

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

				HGpuProgram gpuProgram = Importer::instance().import<GpuProgram>(gpuProgInputLoc, gpuProgImportOptions);
				Resources::instance().save(gpuProgram, gpuProgOutputLoc, true);
			}
		}

		Resources::instance().unloadAllUnused();
	}

	void BuiltinEditorResources::importGUITexture(const WString& name)
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

	HSpriteTexture BuiltinEditorResources::getGUITexture(const WString& name)
	{
		Path texturePath = FileSystem::getWorkingDirectoryPath();
		texturePath.append(DefaultSkinFolder);
		texturePath.append(L"sprite_" + name + L".asset");

		return Resources::instance().load<SpriteTexture>(texturePath);
	}

	HGpuProgram BuiltinEditorResources::getGpuProgram(const WString& name)
	{
		Path programPath = DefaultShaderFolder;
		programPath.append(mActiveShaderSubFolder);
		programPath.append(name + L".asset");

		return gResources().load<GpuProgram>(programPath);
	}

	void BuiltinEditorResources::initDockDropOverlayShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderDockOverlayVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderDockOverlayPSFile);

		mShaderDockOverlay = Shader::create("DockDropOverlayShader");

		mShaderDockOverlay->addParameter("invViewportWidth", "invViewportWidth", GPDT_FLOAT1);
		mShaderDockOverlay->addParameter("invViewportHeight", "invViewportHeight", GPDT_FLOAT1);

		mShaderDockOverlay->addParameter("tintColor", "tintColor", GPDT_FLOAT4);
		mShaderDockOverlay->addParameter("highlightColor", "highlightColor", GPDT_FLOAT4);
		mShaderDockOverlay->addParameter("highlightActive", "highlightActive", GPDT_FLOAT4);

		TechniquePtr newTechnique = mShaderDockOverlay->addTechnique(mActiveRenderSystem, RendererInvariant);
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

	void BuiltinEditorResources::initSceneGridShader()
	{
		HGpuProgram vsProgram = getGpuProgram(SceneGridVSFile);
		HGpuProgram psProgram = getGpuProgram(SceneGridPSFile);

		mShaderSceneGrid = Shader::create("SceneGridShader");

		mShaderSceneGrid->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderSceneGrid->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		BLEND_STATE_DESC blendDesc;
		blendDesc.renderTargetDesc[0].blendEnable = true;
		blendDesc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		blendDesc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		blendDesc.renderTargetDesc[0].blendOp = BO_ADD;
		blendDesc.renderTargetDesc[0].renderTargetWriteMask = 0x7; // Don't write to alpha

		HBlendState blendState = BlendState::create(blendDesc);
		newPass->setBlendState(blendState);

		RASTERIZER_STATE_DESC rasterizerDesc;
		rasterizerDesc.cullMode = CULL_NONE;

		HRasterizerState rasterizerState = RasterizerState::create(rasterizerDesc);
		newPass->setRasterizerState(rasterizerState);
	}

	void BuiltinEditorResources::initPickingShader(CullingMode cullMode)
	{
		UINT32 modeIdx = (UINT32)cullMode;

		HGpuProgram vsProgram = getGpuProgram(PickingVSFile);
		HGpuProgram psProgram = getGpuProgram(PickingPSFile);

		mShaderPicking[modeIdx] = Shader::create("PickingShader");
		mShaderPicking[modeIdx]->addParameter("colorIndex", "colorIndex", GPDT_FLOAT4);
		mShaderPicking[modeIdx]->addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderPicking[modeIdx]->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		RASTERIZER_STATE_DESC rasterizerDesc;
		rasterizerDesc.scissorEnable = true;
		rasterizerDesc.cullMode = cullMode;

		HRasterizerState rasterizerState = RasterizerState::create(rasterizerDesc);
		newPass->setRasterizerState(rasterizerState);
	}

	void BuiltinEditorResources::initPickingAlphaShader(CullingMode cullMode)
	{
		UINT32 modeIdx = (UINT32)cullMode;

		HGpuProgram vsProgram = getGpuProgram(PickingAlphaVSFile);
		HGpuProgram psProgram = getGpuProgram(PickingAlphaPSFile);

		mShaderPickingAlpha[modeIdx] = Shader::create("PickingAlphaShader");

		mShaderPickingAlpha[modeIdx]->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		mShaderPickingAlpha[modeIdx]->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);

		mShaderPickingAlpha[modeIdx]->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		mShaderPickingAlpha[modeIdx]->addParameter("alphaCutoff", "alphaCutoff", GPDT_FLOAT1);
		mShaderPickingAlpha[modeIdx]->addParameter("colorIndex", "colorIndex", GPDT_FLOAT4);
		mShaderPickingAlpha[modeIdx]->addParameter("matWorldViewProj", "matWorldViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderPickingAlpha[modeIdx]->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		RASTERIZER_STATE_DESC rasterizerDesc;
		rasterizerDesc.scissorEnable = true;
		rasterizerDesc.cullMode = cullMode;

		HRasterizerState rasterizerState = RasterizerState::create(rasterizerDesc);
		newPass->setRasterizerState(rasterizerState);
	}

	void BuiltinEditorResources::initWireGizmoShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderWireGizmoVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderWireGizmoPSFile);

		mShaderGizmoWire = Shader::create("GizmoWire");

		mShaderGizmoWire->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderGizmoWire->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);
	}

	void BuiltinEditorResources::initSolidGizmoShader()
	{
		HGpuProgram vsProgram = getGpuProgram(ShaderSolidGizmoVSFile);
		HGpuProgram psProgram = getGpuProgram(ShaderSolidGizmoPSFile);

		mShaderGizmoSolid = Shader::create("GizmoSolid");

		mShaderGizmoSolid->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderGizmoSolid->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);
	}

	void BuiltinEditorResources::initIconGizmoShader()
	{
		HGpuProgram vsProgram0 = getGpuProgram(ShaderIconGizmo0VSFile);
		HGpuProgram psProgram0 = getGpuProgram(ShaderIconGizmo0PSFile);
		HGpuProgram vsProgram1 = getGpuProgram(ShaderIconGizmo1VSFile);
		HGpuProgram psProgram1 = getGpuProgram(ShaderIconGizmo1PSFile);

		mShaderGizmoIcon = Shader::create("GizmoIcon");

		mShaderGizmoIcon->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);
		mShaderGizmoIcon->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		mShaderGizmoIcon->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);
		mShaderGizmoIcon->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		BLEND_STATE_DESC blendDesc;
		blendDesc.renderTargetDesc[0].blendEnable = true;
		blendDesc.renderTargetDesc[0].srcBlend = BF_SOURCE_ALPHA;
		blendDesc.renderTargetDesc[0].dstBlend = BF_INV_SOURCE_ALPHA;
		blendDesc.renderTargetDesc[0].blendOp = BO_ADD;

		HBlendState blendState = BlendState::create(blendDesc);

		RASTERIZER_STATE_DESC rasterizerDesc;
		rasterizerDesc.cullMode = CULL_NONE;

		HRasterizerState rasterizerState = RasterizerState::create(rasterizerDesc);

		DEPTH_STENCIL_STATE_DESC depthStencilState0Desc;
		depthStencilState0Desc.depthWriteEnable = false;
		depthStencilState0Desc.depthComparisonFunc = CMPF_LESS_EQUAL;

		HDepthStencilState depthStencilState0 = DepthStencilState::create(depthStencilState0Desc);

		DEPTH_STENCIL_STATE_DESC depthStencilState1Desc;
		depthStencilState1Desc.depthWriteEnable = false;
		depthStencilState1Desc.depthComparisonFunc = CMPF_GREATER;

		HDepthStencilState depthStencilState1 = DepthStencilState::create(depthStencilState1Desc);

		TechniquePtr newTechnique = mShaderGizmoIcon->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass0 = newTechnique->addPass();
		newPass0->setVertexProgram(vsProgram0);
		newPass0->setFragmentProgram(psProgram0);

		newPass0->setBlendState(blendState);
		newPass0->setDepthStencilState(depthStencilState0);
		newPass0->setRasterizerState(rasterizerState);

		PassPtr newPass1 = newTechnique->addPass();
		newPass1->setVertexProgram(vsProgram1);
		newPass1->setFragmentProgram(psProgram1);

		newPass1->setBlendState(blendState);
		newPass1->setDepthStencilState(depthStencilState1);
		newPass1->setRasterizerState(rasterizerState);
	}

	void BuiltinEditorResources::initGizmoPickingShader()
	{
		HGpuProgram vsProgram = getGpuProgram(GizmoPickingVSFile);
		HGpuProgram psProgram = getGpuProgram(GizmoPickingPSFile);

		mShaderGizmoPicking = Shader::create("GizmoPickingShader");
		mShaderGizmoPicking->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderGizmoPicking->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		RASTERIZER_STATE_DESC rasterizerDesc;
		rasterizerDesc.scissorEnable = true;

		HRasterizerState rasterizerState = RasterizerState::create(rasterizerDesc);
		newPass->setRasterizerState(rasterizerState);
	}

	void BuiltinEditorResources::initGizmoPickingAlphaShader()
	{
		HGpuProgram vsProgram = getGpuProgram(GizmoPickingAlphaVSFile);
		HGpuProgram psProgram = getGpuProgram(GizmoPickingAlphaPSFile);

		mShaderGizmoAlphaPicking = Shader::create("GizmoPickingAlphaShader");

		mShaderGizmoAlphaPicking->addParameter("mainTexSamp", "mainTexSamp", GPOT_SAMPLER2D);
		mShaderGizmoAlphaPicking->addParameter("mainTexSamp", "mainTexture", GPOT_SAMPLER2D);

		mShaderGizmoAlphaPicking->addParameter("mainTexture", "mainTexture", GPOT_TEXTURE2D);

		mShaderGizmoAlphaPicking->addParameter("alphaCutoff", "alphaCutoff", GPDT_FLOAT1);
		mShaderGizmoAlphaPicking->addParameter("matViewProj", "matViewProj", GPDT_MATRIX_4X4);

		TechniquePtr newTechnique = mShaderGizmoAlphaPicking->addTechnique(mActiveRenderSystem, RendererInvariant);
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vsProgram);
		newPass->setFragmentProgram(psProgram);

		RASTERIZER_STATE_DESC rasterizerDesc;
		rasterizerDesc.scissorEnable = true;

		HRasterizerState rasterizerState = RasterizerState::create(rasterizerDesc);
		newPass->setRasterizerState(rasterizerState);
	}

	HMaterial BuiltinEditorResources::createDockDropOverlayMaterial() const
	{
		return Material::create(mShaderDockOverlay);
	}

	HMaterial BuiltinEditorResources::createSceneGridMaterial() const
	{
		return Material::create(mShaderSceneGrid);
	}

	HMaterial BuiltinEditorResources::createPicking(CullingMode cullMode) const
	{
		UINT32 modeIdx = (UINT32)cullMode;

		return Material::create(mShaderPicking[modeIdx]);
	}

	HMaterial BuiltinEditorResources::createPickingAlpha(CullingMode cullMode) const
	{
		UINT32 modeIdx = (UINT32)cullMode;

		return Material::create(mShaderPickingAlpha[modeIdx]);
	}

	HMaterial BuiltinEditorResources::createWireGizmoMat() const
	{
		return Material::create(mShaderGizmoWire);
	}

	HMaterial BuiltinEditorResources::createSolidGizmoMat() const
	{
		return Material::create(mShaderGizmoSolid);
	}

	HMaterial BuiltinEditorResources::createIconGizmoMat() const
	{
		return Material::create(mShaderGizmoIcon);
	}

	HMaterial BuiltinEditorResources::createGizmoPickingMat() const
	{
		return Material::create(mShaderGizmoPicking);
	}

	HMaterial BuiltinEditorResources::createAlphaGizmoPickingMat() const
	{
		return Material::create(mShaderGizmoAlphaPicking);
	}
}