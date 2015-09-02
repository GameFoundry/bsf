#include "BsBuiltinEditorResources.h"
#include "BsBuiltinResources.h"
#include "BsGUIElementStyle.h"

#include "BsGUILabel.h"
#include "BsGUITexture.h"
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
#include "BsGUIProgressBar.h"
#include "BsGUISlider.h"
#include "BsGUIDropDownContent.h"
#include "BsGUIStatusBar.h"

#include "BsFont.h"
#include "BsFontImportOptions.h"
#include "BsImporter.h"
#include "BsGpuProgram.h"
#include "BsShader.h"
#include "BsShaderInclude.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsMaterial.h"
#include "BsBlendState.h"
#include "BsDepthStencilState.h"
#include "BsRasterizerState.h"
#include "BsResources.h"
#include "BsRTTIType.h"
#include "BsFileSystem.h"
#include "BsResourceManifest.h"

namespace BansheeEngine
{
	const String BuiltinEditorResources::ObjectFieldStyleName = "GUIObjectField";
	const String BuiltinEditorResources::ObjectFieldLabelStyleName = "EditorFieldLabel";
	const String BuiltinEditorResources::ObjectFieldDropBtnStyleName = "DropButton";
	const String BuiltinEditorResources::ObjectFieldClearBtnStyleName = "ObjectClearButton";

	const String BuiltinEditorResources::TextureFieldStyleName = "GUITextureField";
	const String BuiltinEditorResources::TextureFieldLabelStyleName = "TextureFieldLabel";
	const String BuiltinEditorResources::TextureFieldDropStyleName = "TextureDrop";
	const String BuiltinEditorResources::TextureFieldClearBtnStyleName = "TextureClearButton";

	const WString BuiltinEditorResources::DefaultFontFilename = L"arial.ttf";
	const UINT32 BuiltinEditorResources::DefaultFontSize = 11;

	const Color BuiltinEditorResources::TextNormalColor = Color(0.7f, 0.7f, 0.7f);
	const Color BuiltinEditorResources::TextActiveColor = Color(0.0f, 0.0f, 0.0f);

	const WString BuiltinEditorResources::GUISkinFile = L"GUISkin";

	const Path BuiltinEditorResources::ShaderFolder = L"Shaders\\";
	const Path BuiltinEditorResources::SkinFolder = L"Skin\\";
	const Path BuiltinEditorResources::IconFolder = L"Skin\\Icons";
	const Path BuiltinEditorResources::ShaderIncludeFolder = L"Includes\\";

	const Path BuiltinEditorResources::BuiltinRawDataFolder = RUNTIME_DATA_PATH + L"Raw\\Editor\\";
	const Path BuiltinEditorResources::EditorRawSkinFolder = BuiltinRawDataFolder + SkinFolder;
	const Path BuiltinEditorResources::EditorRawShaderFolder = BuiltinRawDataFolder + ShaderFolder;
	const Path BuiltinEditorResources::EditorRawShaderIncludeFolder = BuiltinRawDataFolder + ShaderIncludeFolder;

	const Path BuiltinEditorResources::BuiltinDataFolder = RUNTIME_DATA_PATH + L"Editor\\";
	const Path BuiltinEditorResources::EditorSkinFolder = BuiltinDataFolder + SkinFolder;
	const Path BuiltinEditorResources::EditorIconFolder = BuiltinDataFolder + IconFolder;
	const Path BuiltinEditorResources::EditorShaderFolder = BuiltinDataFolder + ShaderFolder;
	const Path BuiltinEditorResources::EditorShaderIncludeFolder = BuiltinDataFolder + ShaderIncludeFolder;

	const Path BuiltinEditorResources::ResourceManifestPath = BuiltinDataFolder + "ResourceManifest.asset";
	const Path BuiltinEditorResources::DefaultWidgetLayoutPath = BuiltinDataFolder + "Layout.asset";

	const WString BuiltinEditorResources::FolderIconTex = L"FolderIcon.psd";
	const WString BuiltinEditorResources::MeshIconTex = L"MeshIcon.psd";
	const WString BuiltinEditorResources::TextureIconTex = L"TextureIcon.psd";
	const WString BuiltinEditorResources::FontIconTex = L"FontIcon.psd";
	const WString BuiltinEditorResources::PlainTextIconTex = L"TextIcon.psd";
	const WString BuiltinEditorResources::ScriptCodeIconTex = L"CSharpIcon.psd";
	const WString BuiltinEditorResources::ShaderIconTex = L"ShaderIcon.psd";
	const WString BuiltinEditorResources::ShaderIncludeIconTex = L"ShaderIncludeIcon.psd";
	const WString BuiltinEditorResources::MaterialIconTex = L"MaterialIcon.psd";
	const WString BuiltinEditorResources::SpriteTextureIconTex = L"SpriteIcon.psd";
	const WString BuiltinEditorResources::PrefabIconTex = L"PrefabIcon.psd";

	const WString BuiltinEditorResources::LogInfoIconTex = L"IconInfo.psd";
	const WString BuiltinEditorResources::LogWarningIconTex = L"IconWarning.psd";
	const WString BuiltinEditorResources::LogErrorIconTex = L"IconError.psd";

	const WString BuiltinEditorResources::ButtonNormalTex = L"ButtonNormal.png";
	const WString BuiltinEditorResources::ButtonHoverTex = L"ButtonHover.png";
	const WString BuiltinEditorResources::ButtonActiveTex = L"ButtonActive.png";

	const WString BuiltinEditorResources::ButtonLeftNormalTex = L"ButtonLeftNormal.png";
	const WString BuiltinEditorResources::ButtonLeftHoverTex = L"ButtonLeftHover.png";
	const WString BuiltinEditorResources::ButtonLeftActiveTex = L"ButtonLeftActive.png";

	const WString BuiltinEditorResources::ButtonRightNormalTex = L"ButtonRightNormal.png";
	const WString BuiltinEditorResources::ButtonRightHoverTex = L"ButtonRightHover.png";
	const WString BuiltinEditorResources::ButtonRightActiveTex = L"ButtonRightActive.png";

	const WString BuiltinEditorResources::ToggleNormalTex = L"ToggleNormal.png";
	const WString BuiltinEditorResources::ToggleHoverTex = L"ToggleHover.png";
	const WString BuiltinEditorResources::ToggleNormalOnTex = L"ToggleNormalOn.png";
	const WString BuiltinEditorResources::ToggleHoverOnTex = L"ToggleHoverOn.png";

	const WString BuiltinEditorResources::InputBoxNormalTex = L"InputBoxNormal.png";
	const WString BuiltinEditorResources::InputBoxHoverTex = L"InputBoxHover.png";
	const WString BuiltinEditorResources::InputBoxFocusedTex = L"InputBoxActive.png";

	const WString BuiltinEditorResources::ObjectDropBtnNormalTex = L"ObjectDropNormal.png";
	const WString BuiltinEditorResources::ObjectDropBtnNormalOnTex = L"ObjectDropNormalOn.png";
	const WString BuiltinEditorResources::ObjectClearBtnNormalTex = L"ObjectDropClearNormal.png";
	const WString BuiltinEditorResources::ObjectClearBtnHoverTex = L"ObjectDropClearHover.png";
	const WString BuiltinEditorResources::ObjectClearBtnActiveTex = L"ObjectDropClearActive.png";
	
	const WString BuiltinEditorResources::DropDownBtnNormalTex = L"DropDownButtonNormal.png";
	const WString BuiltinEditorResources::DropDownBtnHoverTex = L"DropDownButtonHover.png";
	const WString BuiltinEditorResources::DropDownBtnActiveTex = L"DropDownButtonActive.png";

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

	const WString BuiltinEditorResources::WindowBackgroundTex = L"WindowBackground.png";

	const WString BuiltinEditorResources::WindowFrameNormal = L"WindowFrameNormal.png";
	const WString BuiltinEditorResources::WindowFrameFocused = L"WindowFrameFocused.png";

	const WString BuiltinEditorResources::WindowCloseButtonNormal = L"WindowCloseBtnNormal.psd";
	const WString BuiltinEditorResources::WindowCloseButtonHover = L"WindowCloseBtnHover.psd";

	const WString BuiltinEditorResources::WindowMinButtonNormal = L"WindowMinBtnNormal.psd"; 
	const WString BuiltinEditorResources::WindowMinButtonHover = L"WindowMinBtnHover.psd"; 

	const WString BuiltinEditorResources::WindowMaxButtonNormal = L"WindowMaxBtnNormal.psd";
	const WString BuiltinEditorResources::WindowMaxButtonHover = L"WindowMaxBtnHover.psd";

	const WString BuiltinEditorResources::TabBarBackgroundTex = L"TabBarBackground.png";

	const WString BuiltinEditorResources::TabButtonNormalTex = L"TabButtonNormal.png";
	const WString BuiltinEditorResources::TabButtonFocusedTex = L"TabButtonFocused.png";

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

	const WString BuiltinEditorResources::DockSliderNormalTex = L"DockSliderNormal.png";

	const WString BuiltinEditorResources::TreeViewExpandButtonOffNormal = L"TreeViewExpandButtonOffNormal.psd";
	const WString BuiltinEditorResources::TreeViewExpandButtonOffHover = L"TreeViewExpandButtonOffHover.psd";
	const WString BuiltinEditorResources::TreeViewExpandButtonOnNormal = L"TreeViewExpandButtonOnNormal.psd";
	const WString BuiltinEditorResources::TreeViewExpandButtonOnHover = L"TreeViewExpandButtonOnHover.psd";

	const WString BuiltinEditorResources::TreeViewHighlightBackground = L"TreeViewHighlightBackground.psd";
	const WString BuiltinEditorResources::TreeViewEditBox = L"TreeViewEditBox.psd";
	const WString BuiltinEditorResources::TreeViewElementHighlight = L"TreeViewElementHighlight.psd";
	const WString BuiltinEditorResources::TreeViewElementSepHighlight = L"TreeViewElementSepHighlight.psd";

	const WString BuiltinEditorResources::ColorPickerSliderHorzHandleTex = L"ColorPickerSliderHorzHandle.psd";
	const WString BuiltinEditorResources::ColorPickerSliderVertHandleTex = L"ColorPickerSliderVertHandle.psd";
	const WString BuiltinEditorResources::ColorPickerSlider2DHandleTex = L"ColorPicker2DHandle.psd";

	const WString BuiltinEditorResources::ProgressBarFillTex = L"ProgressBarFill.psd";
	const WString BuiltinEditorResources::ProgressBarBgTex = L"ProgressBarBg.psd";

	const WString BuiltinEditorResources::SelectionAreaTex = L"SelectionHighlight.psd";
	const WString BuiltinEditorResources::SelectionBgTex = L"SelectionBg.psd";

	const WString BuiltinEditorResources::TextureDropTex = L"TextureDrop.psd";
	const WString BuiltinEditorResources::TextureDropOnTex = L"TextureDropOn.psd";

	const WString BuiltinEditorResources::XButtonNormalTex = L"XBtnNormal.psd";
	const WString BuiltinEditorResources::XButtonHoverTex = L"XBtnHover.psd";

	const WString BuiltinEditorResources::StatusBarBgTex = L"StatusBarBg.psd";
	const WString BuiltinEditorResources::ScrollAreaBgTex = L"ScrollAreaBg.png";

	/************************************************************************/
	/* 									SHADERS                      		*/
	/************************************************************************/

	const WString BuiltinEditorResources::ShaderDockOverlayFile = L"DockDropOverlay.bsl";
	const WString BuiltinEditorResources::ShaderSceneGridFile = L"SceneGrid.bsl";
	const WString BuiltinEditorResources::ShaderPickingCullNoneFile = L"PickingCullNone.bsl";
	const WString BuiltinEditorResources::ShaderPickingCullCWFile = L"PickingCullCW.bsl";
	const WString BuiltinEditorResources::ShaderPickingCullCCWFile = L"PickingCullCCW.bsl";
	const WString BuiltinEditorResources::ShaderPickingAlphaCullNoneFile = L"PickingAlphaCullNone.bsl";
	const WString BuiltinEditorResources::ShaderPickingAlphaCullCWFile = L"PickingAlphaCullCW.bsl";
	const WString BuiltinEditorResources::ShaderPickingAlphaCullCCWFile = L"PickingAlphaCullCCW.bsl";
	const WString BuiltinEditorResources::ShaderWireGizmoFile = L"WireGizmo.bsl";
	const WString BuiltinEditorResources::ShaderSolidGizmoFile = L"SolidGizmo.bsl";
	const WString BuiltinEditorResources::ShaderWireHandleFile = L"WireHandle.bsl";
	const WString BuiltinEditorResources::ShaderSolidHandleFile = L"SolidHandle.bsl";
	const WString BuiltinEditorResources::ShaderIconGizmoFile = L"IconGizmo.bsl";
	const WString BuiltinEditorResources::ShaderGizmoPickingFile = L"GizmoPicking.bsl";
	const WString BuiltinEditorResources::ShaderGizmoPickingAlphaFile = L"GizmoPickingAlpha.bsl";
	const WString BuiltinEditorResources::ShaderSelectionFile = L"Selection.bsl";

	BuiltinEditorResources::BuiltinEditorResources()
	{
		Path absoluteDataPath = FileSystem::getWorkingDirectoryPath();
		absoluteDataPath.append(BuiltinDataFolder);

#if BS_DEBUG_MODE
		if (BuiltinResourcesHelper::checkForModifications(BuiltinRawDataFolder, BuiltinDataFolder + L"Timestamp.asset"))
		{
			mResourceManifest = ResourceManifest::create("BuiltinResources");
			gResources().registerResourceManifest(mResourceManifest);

			preprocess();
			BuiltinResourcesHelper::writeTimestamp(BuiltinDataFolder + L"Timestamp.asset");

			Path absoluteDataPath = FileSystem::getWorkingDirectoryPath();
			absoluteDataPath.append(BuiltinDataFolder);

			ResourceManifest::save(mResourceManifest, ResourceManifestPath, absoluteDataPath);
		}
#endif

		if (mResourceManifest == nullptr)
		{
			if (FileSystem::exists(ResourceManifestPath))
				mResourceManifest = ResourceManifest::load(ResourceManifestPath, absoluteDataPath);

			if (mResourceManifest == nullptr)
				mResourceManifest = ResourceManifest::create("BuiltinResources");

			gResources().registerResourceManifest(mResourceManifest);
		}

		mShaderDockOverlay = getShader(ShaderDockOverlayFile);
		mShaderSceneGrid = getShader(ShaderSceneGridFile);
		mShaderPicking[(int)CULL_NONE] = getShader(ShaderPickingCullNoneFile);
		mShaderPicking[(int)CULL_CLOCKWISE] = getShader(ShaderPickingCullCWFile);
		mShaderPicking[(int)CULL_COUNTERCLOCKWISE] = getShader(ShaderPickingCullCCWFile);
		mShaderPickingAlpha[(int)CULL_NONE] = getShader(ShaderPickingAlphaCullNoneFile);
		mShaderPickingAlpha[(int)CULL_CLOCKWISE] = getShader(ShaderPickingAlphaCullCWFile);
		mShaderPickingAlpha[(int)CULL_COUNTERCLOCKWISE] = getShader(ShaderPickingAlphaCullCCWFile);
		mShaderGizmoSolid = getShader(ShaderSolidGizmoFile);
		mShaderGizmoWire = getShader(ShaderWireGizmoFile);
		mShaderGizmoIcon = getShader(ShaderIconGizmoFile);
		mShaderGizmoPicking = getShader(ShaderGizmoPickingFile);
		mShaderGizmoAlphaPicking = getShader(ShaderGizmoPickingAlphaFile);
		mShaderHandleSolid = getShader(ShaderSolidHandleFile);
		mShaderHandleWire = getShader(ShaderWireHandleFile);
		mShaderSelection = getShader(ShaderSelectionFile);

		mSkin = gResources().load<GUISkin>(BuiltinDataFolder + (GUISkinFile + L".asset"));
	}

	BuiltinEditorResources::~BuiltinEditorResources()
	{ }

	void BuiltinEditorResources::preprocess()
	{
		FileSystem::remove(EditorShaderIncludeFolder);
		FileSystem::remove(EditorShaderFolder);
		FileSystem::remove(EditorSkinFolder);

		BuiltinResourcesHelper::importAssets(EditorRawShaderIncludeFolder, EditorShaderIncludeFolder, mResourceManifest); // Hidden dependency: Includes must be imported before shaders
		BuiltinResourcesHelper::importAssets(EditorRawShaderFolder, EditorShaderFolder, mResourceManifest);
		BuiltinResourcesHelper::importAssets(EditorRawSkinFolder, EditorSkinFolder, mResourceManifest);

		// Import font
		BuiltinResourcesHelper::importFont(BuiltinRawDataFolder + DefaultFontFilename, BuiltinDataFolder, DefaultFontSize, false, mResourceManifest);

		// Generate & save GUI sprite textures
		BuiltinResourcesHelper::generateSpriteTextures(EditorSkinFolder, mResourceManifest);

		// Generate & save GUI skin
		{
			HGUISkin skin = generateGUISkin();
			Path outputPath = FileSystem::getWorkingDirectoryPath() + BuiltinDataFolder + (GUISkinFile + L".asset");
			Resources::instance().save(skin, outputPath, true);
			mResourceManifest->registerResource(skin.getUUID(), outputPath);
		}

		Resources::instance().unloadAllUnused();
	}

	HGUISkin BuiltinEditorResources::generateGUISkin()
	{
		HGUISkin skin = GUISkin::create();

		Path fontPath = FileSystem::getWorkingDirectoryPath();
		fontPath.append(BuiltinDataFolder);
		fontPath.append(DefaultFontFilename + L".asset");

		HFont font = Resources::instance().load<Font>(fontPath);

		// Blank entry
		GUIElementStyle blankStyle;

		skin->setStyle("Blank", blankStyle);
		skin->setStyle(GUITexture::getGUITypeName(), blankStyle);

		// Label
		GUIElementStyle labelStyle;
		labelStyle.font = font;
		labelStyle.fontSize = DefaultFontSize;
		labelStyle.fixedWidth = false;
		labelStyle.fixedHeight = true;
		labelStyle.height = 14;
		labelStyle.minWidth = 10;
		labelStyle.normal.textColor = TextNormalColor;

		skin->setStyle(GUILabel::getGUITypeName(), labelStyle);

		// Window frame
		GUIElementStyle windowFrameStyle;
		windowFrameStyle.normal.texture = getGUITexture(WindowFrameNormal);
		windowFrameStyle.focused.texture = getGUITexture(WindowFrameFocused);
		windowFrameStyle.border.left = 1;
		windowFrameStyle.border.right = 1;
		windowFrameStyle.border.top = 1;
		windowFrameStyle.border.bottom = 1;

		skin->setStyle("WindowFrame", windowFrameStyle);

		// Button
		GUIElementStyle buttonStyle;
		buttonStyle.normal.texture = getGUITexture(ButtonNormalTex);
		buttonStyle.hover.texture = getGUITexture(ButtonHoverTex);
		buttonStyle.active.texture = getGUITexture(ButtonActiveTex);
		buttonStyle.normalOn.texture = getGUITexture(ButtonActiveTex);
		buttonStyle.hoverOn.texture = getGUITexture(ButtonActiveTex);
		buttonStyle.activeOn.texture = getGUITexture(ButtonActiveTex);
		buttonStyle.normal.textColor = TextNormalColor;
		buttonStyle.hover.textColor = TextNormalColor;
		buttonStyle.active.textColor = TextActiveColor;
		buttonStyle.normalOn.textColor = TextActiveColor;
		buttonStyle.hoverOn.textColor = TextActiveColor;
		buttonStyle.activeOn.textColor = TextActiveColor;
		buttonStyle.border.left = 2;
		buttonStyle.border.right = 2;
		buttonStyle.border.top = 2;
		buttonStyle.border.bottom = 4;
		buttonStyle.contentOffset.top = 2;
		buttonStyle.contentOffset.left = 3;
		buttonStyle.contentOffset.right = 3;
		buttonStyle.margins.bottom = 2;
		buttonStyle.fixedHeight = true;
		buttonStyle.height = 21;
		buttonStyle.minWidth = 20;
		buttonStyle.font = font;
		buttonStyle.fontSize = DefaultFontSize;
		buttonStyle.textHorzAlign = THA_Center;
		buttonStyle.textVertAlign = TVA_Center;

		skin->setStyle(GUIButton::getGUITypeName(), buttonStyle);

		// Left edge button
		GUIElementStyle leftButtonStyle;
		leftButtonStyle.normal.texture = getGUITexture(ButtonLeftNormalTex);
		leftButtonStyle.hover.texture = getGUITexture(ButtonLeftHoverTex);
		leftButtonStyle.active.texture = getGUITexture(ButtonLeftActiveTex);
		leftButtonStyle.normalOn.texture = getGUITexture(ButtonLeftActiveTex);
		leftButtonStyle.hoverOn.texture = getGUITexture(ButtonLeftActiveTex);
		leftButtonStyle.activeOn.texture = getGUITexture(ButtonLeftActiveTex);
		leftButtonStyle.normal.textColor = TextNormalColor;
		leftButtonStyle.hover.textColor = TextNormalColor;
		leftButtonStyle.active.textColor = TextActiveColor;
		leftButtonStyle.normalOn.textColor = TextActiveColor;
		leftButtonStyle.hoverOn.textColor = TextActiveColor;
		leftButtonStyle.activeOn.textColor = TextActiveColor;
		leftButtonStyle.border.left = 7;
		leftButtonStyle.border.right = 2;
		leftButtonStyle.border.top = 2;
		leftButtonStyle.border.bottom = 4;
		leftButtonStyle.contentOffset.top = 2;
		leftButtonStyle.contentOffset.left = 7;
		leftButtonStyle.contentOffset.right = 3;
		leftButtonStyle.margins.bottom = 2;
		leftButtonStyle.fixedHeight = true;
		leftButtonStyle.height = 21;
		leftButtonStyle.minWidth = 20;
		leftButtonStyle.font = font;
		leftButtonStyle.fontSize = DefaultFontSize;
		leftButtonStyle.textHorzAlign = THA_Center;
		leftButtonStyle.textVertAlign = TVA_Center;

		skin->setStyle("ButtonLeft", leftButtonStyle);

		// Right edge button
		GUIElementStyle rightButtonStyle;
		rightButtonStyle.normal.texture = getGUITexture(ButtonRightNormalTex);
		rightButtonStyle.hover.texture = getGUITexture(ButtonRightHoverTex);
		rightButtonStyle.active.texture = getGUITexture(ButtonRightActiveTex);
		rightButtonStyle.normalOn.texture = getGUITexture(ButtonRightActiveTex);
		rightButtonStyle.hoverOn.texture = getGUITexture(ButtonRightActiveTex);
		rightButtonStyle.activeOn.texture = getGUITexture(ButtonRightActiveTex);
		rightButtonStyle.normal.textColor = TextNormalColor;
		rightButtonStyle.hover.textColor = TextNormalColor;
		rightButtonStyle.active.textColor = TextActiveColor;
		rightButtonStyle.normalOn.textColor = TextActiveColor;
		rightButtonStyle.hoverOn.textColor = TextActiveColor;
		rightButtonStyle.activeOn.textColor = TextActiveColor;
		rightButtonStyle.border.left = 2;
		rightButtonStyle.border.right = 7;
		rightButtonStyle.border.top = 2;
		rightButtonStyle.border.bottom = 4;
		rightButtonStyle.contentOffset.top = 2;
		rightButtonStyle.contentOffset.left = 3;
		rightButtonStyle.contentOffset.right = 7;
		rightButtonStyle.margins.bottom = 2;
		rightButtonStyle.fixedHeight = true;
		rightButtonStyle.height = 21;
		rightButtonStyle.minWidth = 20;
		rightButtonStyle.font = font;
		rightButtonStyle.fontSize = DefaultFontSize;
		rightButtonStyle.textHorzAlign = THA_Center;
		rightButtonStyle.textVertAlign = TVA_Center;

		skin->setStyle("ButtonRight", rightButtonStyle);

		// Toggle
		GUIElementStyle toggleStyle;
		toggleStyle.normal.texture = getGUITexture(ToggleNormalTex);
		toggleStyle.hover.texture = getGUITexture(ToggleHoverTex);
		toggleStyle.active.texture = toggleStyle.hover.texture;
		toggleStyle.normalOn.texture = getGUITexture(ToggleNormalOnTex);
		toggleStyle.hoverOn.texture = getGUITexture(ToggleHoverOnTex);
		toggleStyle.activeOn.texture = toggleStyle.hoverOn.texture;
		toggleStyle.fixedHeight = true;
		toggleStyle.fixedWidth = true;
		toggleStyle.margins.bottom = 2;
		toggleStyle.height = 15;
		toggleStyle.width = 13;

		skin->setStyle(GUIToggle::getGUITypeName(), toggleStyle);

		// Color
		GUIElementStyle colorStyle;
		colorStyle.margins.left = 2;
		colorStyle.margins.right = 2;
		colorStyle.margins.top = 2;
		colorStyle.margins.bottom = 2;
		colorStyle.fixedHeight = true;
		colorStyle.height = 10;
		colorStyle.minWidth = 10;

		skin->setStyle(GUIColor::getGUITypeName(), colorStyle);

		// Window background texture
		GUIElementStyle windowBgStyle;
		windowBgStyle.normal.texture = getGUITexture(WindowBackgroundTex);

		skin->setStyle("WindowBackground", windowBgStyle);

		// Window title bar background
		GUIElementStyle titleBarBgStyle;
		titleBarBgStyle.normal.texture = getGUITexture(TabBarBackgroundTex);
		titleBarBgStyle.fixedHeight = true;
		titleBarBgStyle.height = 16;

		skin->setStyle("TitleBarBackground", titleBarBgStyle);

		// Tabbed title bar tab button
		GUIElementStyle tabbedBarButton;
		tabbedBarButton.normal.texture = getGUITexture(TabButtonNormalTex);
		tabbedBarButton.hover.texture = tabbedBarButton.normal.texture;
		tabbedBarButton.active.texture = getGUITexture(TabButtonFocusedTex);
		tabbedBarButton.normalOn.texture = tabbedBarButton.active.texture;
		tabbedBarButton.hoverOn.texture = tabbedBarButton.active.texture;
		tabbedBarButton.activeOn.texture = tabbedBarButton.active.texture;
		tabbedBarButton.normal.textColor = TextNormalColor;
		tabbedBarButton.hover.textColor = TextNormalColor;
		tabbedBarButton.active.textColor = TextActiveColor;
		tabbedBarButton.normalOn.textColor = TextActiveColor;
		tabbedBarButton.hoverOn.textColor = TextActiveColor;
		tabbedBarButton.activeOn.textColor = TextActiveColor;
		tabbedBarButton.border.left = 12;
		tabbedBarButton.border.right = 12;
		tabbedBarButton.margins.left = 5;
		tabbedBarButton.margins.right = 5;
		tabbedBarButton.contentOffset.top = 3;
		tabbedBarButton.contentOffset.left = 6;
		tabbedBarButton.contentOffset.right = 6;
		tabbedBarButton.fixedHeight = true;
		tabbedBarButton.height = 15;
		tabbedBarButton.minWidth = 24;
		tabbedBarButton.maxWidth = 110;
		tabbedBarButton.font = font;
		tabbedBarButton.fontSize = DefaultFontSize;
		tabbedBarButton.textHorzAlign = THA_Center;
		tabbedBarButton.textVertAlign = TVA_Center;

		skin->setStyle("TabbedBarBtn", tabbedBarButton);

		// Tabbed title bar drag/drop button
		GUIElementStyle tabbedBarDropButton;
		tabbedBarDropButton.fixedHeight = true;
		tabbedBarDropButton.fixedWidth = true;
		tabbedBarDropButton.height = 13;
		tabbedBarDropButton.width = 6;

		skin->setStyle("TabbedBarDropArea", tabbedBarDropButton);

		// Window minimize button
		GUIElementStyle winMinButtonStyle;
		winMinButtonStyle.normal.texture = getGUITexture(WindowMinButtonNormal);
		winMinButtonStyle.hover.texture = getGUITexture(WindowMinButtonHover);
		winMinButtonStyle.active.texture = winMinButtonStyle.hover.texture;
		winMinButtonStyle.fixedHeight = true;
		winMinButtonStyle.fixedWidth = true;
		winMinButtonStyle.height = 7;
		winMinButtonStyle.width = 8;

		skin->setStyle("WinMinimizeBtn", winMinButtonStyle);

		// Window maximize button
		GUIElementStyle winMaxButtonStyle;
		winMaxButtonStyle.normal.texture = getGUITexture(WindowMaxButtonNormal);
		winMaxButtonStyle.hover.texture = getGUITexture(WindowMaxButtonHover);
		winMaxButtonStyle.active.texture = winMaxButtonStyle.hover.texture;
		winMaxButtonStyle.fixedHeight = true;
		winMaxButtonStyle.fixedWidth = true;
		winMaxButtonStyle.height = 8;
		winMaxButtonStyle.width = 8;

		skin->setStyle("WinMaximizeBtn", winMaxButtonStyle);

		// Window close button
		GUIElementStyle winCloseButtonStyle;
		winCloseButtonStyle.normal.texture = getGUITexture(WindowCloseButtonNormal);
		winCloseButtonStyle.hover.texture = getGUITexture(WindowCloseButtonHover);
		winCloseButtonStyle.active.texture = winCloseButtonStyle.hover.texture;
		winCloseButtonStyle.fixedHeight = true;
		winCloseButtonStyle.fixedWidth = true;
		winCloseButtonStyle.height = 7;
		winCloseButtonStyle.width = 8;

		skin->setStyle("WinCloseBtn", winCloseButtonStyle);

		// Input box
		GUIElementStyle inputBoxStyle;
		inputBoxStyle.normal.texture = getGUITexture(InputBoxNormalTex);
		inputBoxStyle.hover.texture = getGUITexture(InputBoxHoverTex);
		inputBoxStyle.focused.texture = getGUITexture(InputBoxFocusedTex);
		inputBoxStyle.active.texture = inputBoxStyle.normal.texture;
		inputBoxStyle.normal.textColor = TextNormalColor;
		inputBoxStyle.hover.textColor = TextNormalColor;
		inputBoxStyle.focused.textColor = TextNormalColor;
		inputBoxStyle.active.textColor = TextNormalColor;
		inputBoxStyle.border.left = 4;
		inputBoxStyle.border.right = 4;
		inputBoxStyle.border.top = 4;
		inputBoxStyle.border.bottom = 6;
		inputBoxStyle.contentOffset.left = 4;
		inputBoxStyle.contentOffset.right = 4;
		inputBoxStyle.contentOffset.top = 4;
		inputBoxStyle.contentOffset.bottom = 4;
		inputBoxStyle.margins.bottom = 2;
		inputBoxStyle.fixedHeight = true;
		inputBoxStyle.height = 21;
		inputBoxStyle.minWidth = 10;
		inputBoxStyle.font = font;
		inputBoxStyle.fontSize = DefaultFontSize;
		inputBoxStyle.textHorzAlign = THA_Left;
		inputBoxStyle.textVertAlign = TVA_Top;

		skin->setStyle(GUIInputBox::getGUITypeName(), inputBoxStyle);

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

		skin->setStyle("ScrollUpBtn", scrollUpBtnStyle);

		// Down button
		GUIElementStyle scrollDownBtnStyle;
		scrollDownBtnStyle.normal.texture = getGUITexture(ScrollBarDownNormalTex);
		scrollDownBtnStyle.hover.texture = getGUITexture(ScrollBarDownHoverTex);
		scrollDownBtnStyle.active.texture = getGUITexture(ScrollBarDownActiveTex);
		scrollDownBtnStyle.fixedHeight = true;
		scrollDownBtnStyle.fixedWidth = true;
		scrollDownBtnStyle.height = 4;
		scrollDownBtnStyle.width = 8;

		skin->setStyle("ScrollDownBtn", scrollDownBtnStyle);

		// Left button
		GUIElementStyle scrollLeftBtnStyle;
		scrollLeftBtnStyle.normal.texture = getGUITexture(ScrollBarLeftNormalTex);
		scrollLeftBtnStyle.hover.texture = getGUITexture(ScrollBarLeftHoverTex);
		scrollLeftBtnStyle.active.texture = getGUITexture(ScrollBarLeftActiveTex);
		scrollLeftBtnStyle.fixedHeight = true;
		scrollLeftBtnStyle.fixedWidth = true;
		scrollLeftBtnStyle.height = 8;
		scrollLeftBtnStyle.width = 4;

		skin->setStyle("ScrollLeftBtn", scrollLeftBtnStyle);

		// Right button
		GUIElementStyle scrollRightBtnStyle;
		scrollRightBtnStyle.normal.texture = getGUITexture(ScrollBarRightNormalTex);
		scrollRightBtnStyle.hover.texture = getGUITexture(ScrollBarRightHoverTex);
		scrollRightBtnStyle.active.texture = getGUITexture(ScrollBarRightActiveTex);
		scrollRightBtnStyle.fixedHeight = true;
		scrollRightBtnStyle.fixedWidth = true;
		scrollRightBtnStyle.height = 8;
		scrollRightBtnStyle.width = 4;

		skin->setStyle("ScrollRightBtn", scrollRightBtnStyle);

		// Horizontal handle
		GUIElementStyle scrollBarHorzBtnStyle;
		scrollBarHorzBtnStyle.normal.texture = getGUITexture(ScrollBarHandleHorzNormalTex);
		scrollBarHorzBtnStyle.hover.texture = getGUITexture(ScrollBarHandleHorzHoverTex);
		scrollBarHorzBtnStyle.active.texture = getGUITexture(ScrollBarHandleHorzActiveTex);
		scrollBarHorzBtnStyle.fixedHeight = true;
		scrollBarHorzBtnStyle.fixedWidth = true;
		scrollBarHorzBtnStyle.height = 6;
		scrollBarHorzBtnStyle.width = 4;

		skin->setStyle("ScrollBarHorzBtn", scrollBarHorzBtnStyle);

		// Vertical handle
		GUIElementStyle scrollBarVertBtnStyle;
		scrollBarVertBtnStyle.normal.texture = getGUITexture(ScrollBarHandleVertNormalTex);
		scrollBarVertBtnStyle.hover.texture = getGUITexture(ScrollBarHandleVertHoverTex);
		scrollBarVertBtnStyle.active.texture = getGUITexture(ScrollBarHandleVertActiveTex);
		scrollBarVertBtnStyle.fixedHeight = true;
		scrollBarVertBtnStyle.fixedWidth = true;
		scrollBarVertBtnStyle.height = 4;
		scrollBarVertBtnStyle.width = 6;

		skin->setStyle("ScrollBarVertBtn", scrollBarVertBtnStyle);

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

		skin->setStyle("ScrollBarVert", vertScrollBarStyle);

		// Horizontal scroll bar
		GUIElementStyle horzScrollBarStyle;
		horzScrollBarStyle.normal.texture = scrollBarBgPtr;
		horzScrollBarStyle.hover.texture = scrollBarBgPtr;
		horzScrollBarStyle.active.texture = scrollBarBgPtr;
		horzScrollBarStyle.fixedHeight = true;
		horzScrollBarStyle.fixedWidth = false;
		horzScrollBarStyle.minWidth = 16;
		horzScrollBarStyle.height = 8;

		skin->setStyle("ScrollBarHorz", horzScrollBarStyle);

		/************************************************************************/
		/* 								DROP DOWN BOX                      		*/
		/************************************************************************/

		// ListBox button
		GUIElementStyle dropDownListStyle;
		dropDownListStyle.normal.texture = getGUITexture(DropDownBtnNormalTex);
		dropDownListStyle.hover.texture = getGUITexture(DropDownBtnHoverTex);
		dropDownListStyle.active.texture = getGUITexture(DropDownBtnActiveTex);
		dropDownListStyle.normalOn.texture = dropDownListStyle.active.texture;
		dropDownListStyle.hoverOn.texture = dropDownListStyle.active.texture;
		dropDownListStyle.activeOn.texture = dropDownListStyle.active.texture;
		dropDownListStyle.normal.textColor = TextNormalColor;
		dropDownListStyle.hover.textColor = TextNormalColor;
		dropDownListStyle.active.textColor = TextNormalColor;
		dropDownListStyle.normalOn.textColor = TextNormalColor;
		dropDownListStyle.hoverOn.textColor = TextNormalColor;
		dropDownListStyle.activeOn.textColor = TextNormalColor;
		dropDownListStyle.fixedHeight = true;
		dropDownListStyle.fixedWidth = false;
		dropDownListStyle.height = 21;
		dropDownListStyle.minWidth = 20;
		dropDownListStyle.contentOffset.left = 3;
		dropDownListStyle.contentOffset.right = 18;
		dropDownListStyle.contentOffset.top = 2;
		dropDownListStyle.contentOffset.bottom = 2;
		dropDownListStyle.border.left = 2;
		dropDownListStyle.border.right = 16;
		dropDownListStyle.border.top = 2;
		dropDownListStyle.border.bottom = 4;
		dropDownListStyle.margins.bottom = 2;
		dropDownListStyle.font = font;
		dropDownListStyle.fontSize = DefaultFontSize;
		dropDownListStyle.textHorzAlign = THA_Left;
		dropDownListStyle.textVertAlign = TVA_Center;

		skin->setStyle("ListBox", dropDownListStyle);

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

		skin->setStyle("ListBoxScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);
		skin->setStyle("MenuBarScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);
		skin->setStyle("ContextMenuScrollUpBtnArrow", dropDownScrollUpBtnArrowStyle);

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

		skin->setStyle("ListBoxScrollUpBtn", dropDownScrollUpBtnStyle);
		skin->setStyle("MenuBarScrollUpBtn", dropDownScrollUpBtnStyle);
		skin->setStyle("ContextMenuScrollUpBtn", dropDownScrollUpBtnStyle);

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

		skin->setStyle("ListBoxScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);
		skin->setStyle("MenuBarScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);
		skin->setStyle("ContextMenuScrollDownBtnArrow", dropDownScrollDownBtnArrowStyle);

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

		skin->setStyle("ListBoxScrollDownBtn", dropDownScrollDownBtnStyle);
		skin->setStyle("MenuBarScrollDownBtn", dropDownScrollDownBtnStyle);
		skin->setStyle("ContextMenuScrollDownBtn", dropDownScrollDownBtnStyle);

		// DropDown entry button
		GUIElementStyle dropDownEntryBtnStyle;
		dropDownEntryBtnStyle.normal.texture = getGUITexture(DropDownBoxEntryNormalTex);
		dropDownEntryBtnStyle.hover.texture = getGUITexture(DropDownBoxEntryHoverTex);
		dropDownEntryBtnStyle.active.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownEntryBtnStyle.normalOn.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownEntryBtnStyle.hoverOn.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownEntryBtnStyle.activeOn.texture = dropDownEntryBtnStyle.hover.texture;
		dropDownEntryBtnStyle.normal.textColor = TextNormalColor;
		dropDownEntryBtnStyle.hover.textColor = TextNormalColor;
		dropDownEntryBtnStyle.active.textColor = TextNormalColor;
		dropDownEntryBtnStyle.normalOn.textColor = TextNormalColor;
		dropDownEntryBtnStyle.hoverOn.textColor = TextNormalColor;
		dropDownEntryBtnStyle.activeOn.textColor = TextNormalColor;
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

		skin->setStyle(GUIDropDownContent::ENTRY_STYLE_TYPE, dropDownEntryBtnStyle);

		// DropDown entry button with expand
		GUIElementStyle dropDownEntryExpBtnStyle;
		dropDownEntryExpBtnStyle.normal.texture = getGUITexture(DropDownBoxEntryExpNormalTex);
		dropDownEntryExpBtnStyle.hover.texture = getGUITexture(DropDownBoxEntryExpHoverTex);
		dropDownEntryExpBtnStyle.active.texture = dropDownEntryExpBtnStyle.hover.texture;
		dropDownEntryExpBtnStyle.normalOn.texture = dropDownEntryExpBtnStyle.hover.texture;
		dropDownEntryExpBtnStyle.hoverOn.texture = dropDownEntryExpBtnStyle.hover.texture;
		dropDownEntryExpBtnStyle.activeOn.texture = dropDownEntryExpBtnStyle.hover.texture;
		dropDownEntryExpBtnStyle.normal.textColor = TextNormalColor;
		dropDownEntryExpBtnStyle.hover.textColor = TextNormalColor;
		dropDownEntryExpBtnStyle.active.textColor = TextNormalColor;
		dropDownEntryExpBtnStyle.normalOn.textColor = TextNormalColor;
		dropDownEntryExpBtnStyle.hoverOn.textColor = TextNormalColor;
		dropDownEntryExpBtnStyle.activeOn.textColor = TextNormalColor;
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

		skin->setStyle(GUIDropDownContent::ENTRY_EXP_STYLE_TYPE, dropDownEntryExpBtnStyle);

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

		skin->setStyle(GUIDropDownContent::SEPARATOR_STYLE_TYPE, dropDownSeparatorStyle);

		// Drop down content
		GUIElementStyle dropDownContentStyle;
		dropDownContentStyle.minWidth = 50;
		dropDownContentStyle.minHeight = 20;
		dropDownContentStyle.subStyles[GUIDropDownContent::ENTRY_STYLE_TYPE] = GUIDropDownContent::ENTRY_STYLE_TYPE;
		dropDownContentStyle.subStyles[GUIDropDownContent::ENTRY_EXP_STYLE_TYPE] = GUIDropDownContent::ENTRY_EXP_STYLE_TYPE;
		dropDownContentStyle.subStyles[GUIDropDownContent::SEPARATOR_STYLE_TYPE] = GUIDropDownContent::SEPARATOR_STYLE_TYPE;

		skin->setStyle("ListBoxContent", dropDownContentStyle);
		skin->setStyle("MenuBarContent", dropDownContentStyle);
		skin->setStyle("ContextMenuContent", dropDownContentStyle);

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

		skin->setStyle("ListBoxFrame", dropDownBoxStyle);
		skin->setStyle("MenuBarFrame", dropDownBoxStyle);
		skin->setStyle("ContextMenuFrame", dropDownBoxStyle);

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

		skin->setStyle("MenuBarBg", menuBarBgStyle);

		// MenuBar Banshee logo
		GUIElementStyle menuBarBansheeLogoStyle;
		menuBarBansheeLogoStyle.normal.texture = getGUITexture(MenuBarBansheeLogoTex);
		menuBarBansheeLogoStyle.fixedHeight = true;
		menuBarBansheeLogoStyle.fixedWidth = true;
		menuBarBansheeLogoStyle.height = 7;
		menuBarBansheeLogoStyle.width = 51;

		skin->setStyle("MenuBarBansheeLogo", menuBarBansheeLogoStyle);

		// MenuBar button
		GUIElementStyle menuBarBtnStyle;
		menuBarBtnStyle.normal.texture = getGUITexture(MenuBarBtnNormalTex);
		menuBarBtnStyle.hover.texture = getGUITexture(MenuBarBtnHoverTex);
		menuBarBtnStyle.active.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.normalOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.hoverOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.activeOn.texture = menuBarBtnStyle.hover.texture;
		menuBarBtnStyle.normal.textColor = TextNormalColor;
		menuBarBtnStyle.hover.textColor = TextNormalColor;
		menuBarBtnStyle.active.textColor = TextActiveColor;
		menuBarBtnStyle.normalOn.textColor = TextActiveColor;
		menuBarBtnStyle.hoverOn.textColor = TextActiveColor;
		menuBarBtnStyle.activeOn.textColor = TextActiveColor;
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

		skin->setStyle("MenuBarBtn", menuBarBtnStyle);

		/************************************************************************/
		/* 								DOCK SLIDER	                     		*/
		/************************************************************************/

		GUIElementStyle dockSliderBtnStyle;
		dockSliderBtnStyle.normal.texture = getGUITexture(DockSliderNormalTex);
		dockSliderBtnStyle.fixedHeight = false;
		dockSliderBtnStyle.fixedWidth = false;
		dockSliderBtnStyle.height = 2;
		dockSliderBtnStyle.width = 2;

		skin->setStyle("DockSliderBtn", dockSliderBtnStyle);

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

		skin->setStyle("TreeViewFoldoutBtn", treeViewExpandButtonStyle);

		// Entry
		GUIElementStyle treeViewEntryStyle;
		treeViewEntryStyle.font = font;
		treeViewEntryStyle.fontSize = DefaultFontSize;
		treeViewEntryStyle.fixedWidth = false;
		treeViewEntryStyle.fixedHeight = true;
		treeViewEntryStyle.height = 16;
		treeViewEntryStyle.minWidth = 10;
		treeViewEntryStyle.normal.textColor = TextNormalColor;

		skin->setStyle("TreeViewElementBtn", treeViewEntryStyle);

		// Selection background
		GUIElementStyle treeViewSelBackgroundStyle;
		treeViewSelBackgroundStyle.normal.texture = getGUITexture(SelectionBgTex);
		treeViewSelBackgroundStyle.fixedHeight = false;
		treeViewSelBackgroundStyle.fixedWidth = false;
		treeViewSelBackgroundStyle.height = 2;
		treeViewSelBackgroundStyle.width = 2;

		skin->setStyle("TreeViewSelectionBackground", treeViewSelBackgroundStyle);

		// Highlight background
		GUIElementStyle treeViewHLBackgroundStyle;
		treeViewHLBackgroundStyle.normal.texture = getGUITexture(TreeViewHighlightBackground);
		treeViewHLBackgroundStyle.fixedHeight = false;
		treeViewHLBackgroundStyle.fixedWidth = false;
		treeViewHLBackgroundStyle.height = 2;
		treeViewHLBackgroundStyle.width = 2;

		skin->setStyle("TreeViewHighlightBackground", treeViewHLBackgroundStyle);

		// Edit box
		GUIElementStyle treeViewEditBox;
		treeViewEditBox.normal.texture = getGUITexture(TreeViewEditBox);
		treeViewEditBox.hover.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.focused.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.active.texture = treeViewEditBox.normal.texture;
		treeViewEditBox.normal.textColor = TextNormalColor;
		treeViewEditBox.hover.textColor = TextNormalColor;
		treeViewEditBox.focused.textColor = TextNormalColor;
		treeViewEditBox.active.textColor = TextNormalColor;
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

		skin->setStyle(GUITreeViewEditBox::getGUITypeName(), treeViewEditBox);

		// Element highlight
		GUIElementStyle treeViewElementHighlight;
		treeViewElementHighlight.normal.texture = getGUITexture(TreeViewElementHighlight);
		treeViewElementHighlight.border.left = 1;
		treeViewElementHighlight.border.right = 1;
		treeViewElementHighlight.border.top = 1;
		treeViewElementHighlight.border.bottom = 1;

		skin->setStyle("TreeViewElementHighlight", treeViewElementHighlight);

		// Element separator highlight
		GUIElementStyle treeViewElementSepHighlight;
		treeViewElementSepHighlight.normal.texture = getGUITexture(TreeViewElementSepHighlight);
		treeViewElementSepHighlight.border.left = 1;
		treeViewElementSepHighlight.border.right = 1;
		treeViewElementSepHighlight.border.top = 1;
		treeViewElementSepHighlight.border.bottom = 1;

		skin->setStyle("TreeViewElementSepHighlight", treeViewElementSepHighlight);

		/************************************************************************/
		/* 							OBJECT DROP FIELD                      		*/
		/************************************************************************/
		GUIElementStyle objectDropStyle;
		objectDropStyle.normal.texture = getGUITexture(ObjectDropBtnNormalTex);
		objectDropStyle.normalOn.texture = getGUITexture(ObjectDropBtnNormalOnTex);
		objectDropStyle.normal.textColor = TextNormalColor;
		objectDropStyle.hover.textColor = TextNormalColor;
		objectDropStyle.active.textColor = TextNormalColor;
		objectDropStyle.normalOn.textColor = TextNormalColor;
		objectDropStyle.hoverOn.textColor = TextNormalColor;
		objectDropStyle.activeOn.textColor = TextNormalColor;
		objectDropStyle.border.left = 2;
		objectDropStyle.border.top = 2;
		objectDropStyle.border.bottom = 4;
		objectDropStyle.contentOffset.left = 3;
		objectDropStyle.contentOffset.right = 1;
		objectDropStyle.margins.bottom = 2;
		objectDropStyle.fixedHeight = true;
		objectDropStyle.height = 21;
		objectDropStyle.minWidth = 20;
		objectDropStyle.font = font;
		objectDropStyle.fontSize = DefaultFontSize;
		objectDropStyle.textHorzAlign = THA_Center;
		objectDropStyle.textVertAlign = TVA_Center;

		skin->setStyle(ObjectFieldDropBtnStyleName, objectDropStyle);

		GUIElementStyle objectClearBtnStyle;
		objectClearBtnStyle.normal.texture = getGUITexture(ObjectClearBtnNormalTex);
		objectClearBtnStyle.hover.texture = getGUITexture(ObjectClearBtnHoverTex);
		objectClearBtnStyle.active.texture = getGUITexture(ObjectClearBtnActiveTex);
		objectClearBtnStyle.fixedHeight = true;
		objectClearBtnStyle.fixedWidth = true;
		objectClearBtnStyle.height = 21;
		objectClearBtnStyle.width = 16;
		objectClearBtnStyle.margins.bottom = 2;

		skin->setStyle(ObjectFieldClearBtnStyleName, objectClearBtnStyle);

		GUIElementStyle editorObjectFieldStyle;
		editorObjectFieldStyle.fixedHeight = true;
		editorObjectFieldStyle.height = 15;
		editorObjectFieldStyle.minWidth = 30;
		editorObjectFieldStyle.subStyles[ObjectFieldLabelStyleName] = GUITextField::getLabelStyleType();
		editorObjectFieldStyle.subStyles[ObjectFieldDropBtnStyleName] = ObjectFieldDropBtnStyleName;
		editorObjectFieldStyle.subStyles[ObjectFieldClearBtnStyleName] = ObjectFieldClearBtnStyleName;

		skin->setStyle(ObjectFieldStyleName, editorObjectFieldStyle);

		/************************************************************************/
		/* 						TEXTURE DROP FIELD                      		*/
		/************************************************************************/
		GUIElementStyle textureFieldStyle;
		textureFieldStyle.minHeight = 15;
		textureFieldStyle.minWidth = 15;
		textureFieldStyle.subStyles[TextureFieldDropStyleName] = TextureFieldDropStyleName;
		textureFieldStyle.subStyles[TextureFieldClearBtnStyleName] = TextureFieldClearBtnStyleName;
		textureFieldStyle.subStyles[TextureFieldLabelStyleName] = GUITextField::getLabelStyleType();

		skin->setStyle(TextureFieldStyleName, textureFieldStyle);

		GUIElementStyle textureDropStyle;
		textureDropStyle.font = font;
		textureDropStyle.fontSize = DefaultFontSize;
		textureDropStyle.textHorzAlign = THA_Center;
		textureDropStyle.textVertAlign = TVA_Center;
		textureDropStyle.normal.textColor = Color(95 / 255.0f, 95 / 255.0f, 95 / 255.0f, 1.0f);
		textureDropStyle.normal.texture = getGUITexture(TextureDropTex);
		textureDropStyle.hover.textColor = textureDropStyle.normal.textColor;
		textureDropStyle.hover.texture = textureDropStyle.normal.texture;
		textureDropStyle.active.textColor = textureDropStyle.normal.textColor;
		textureDropStyle.active.texture = textureDropStyle.normal.texture;
		textureDropStyle.normalOn.textColor = Color(95 / 255.0f, 95 / 255.0f, 95 / 255.0f, 1.0f);
		textureDropStyle.normalOn.texture = getGUITexture(TextureDropOnTex);
		textureDropStyle.hoverOn.textColor = textureDropStyle.normalOn.textColor;
		textureDropStyle.hoverOn.texture = textureDropStyle.normalOn.texture;
		textureDropStyle.activeOn.textColor = textureDropStyle.normalOn.textColor;
		textureDropStyle.activeOn.texture = textureDropStyle.normalOn.texture;
		textureDropStyle.height = 82;
		textureDropStyle.width = 82;
		textureDropStyle.fixedHeight = true;
		textureDropStyle.fixedWidth = true;
		textureDropStyle.border.left = 2;
		textureDropStyle.border.right = 2;
		textureDropStyle.border.top = 2;
		textureDropStyle.border.bottom = 2;

		skin->setStyle(TextureFieldDropStyleName, textureDropStyle);
		
		GUIElementStyle textureDropClearStyle;
		textureDropClearStyle.normal.texture = getGUITexture(XButtonNormalTex);
		textureDropClearStyle.hover.texture = getGUITexture(XButtonHoverTex);
		textureDropClearStyle.active.texture = textureDropStyle.hover.texture;
		textureDropClearStyle.fixedWidth = true;
		textureDropClearStyle.fixedHeight = true;
		textureDropClearStyle.height = 11;
		textureDropClearStyle.width = 11;

		skin->setStyle(TextureFieldClearBtnStyleName, textureDropClearStyle);

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

		skin->setStyle(GUITextField::getLabelStyleType(), editorFieldLabelStyle);

		GUIElementStyle editorIntFieldStyle;
		editorIntFieldStyle.fixedHeight = true;
		editorIntFieldStyle.height = 21;
		editorIntFieldStyle.minWidth = 30;
		editorIntFieldStyle.subStyles[GUIIntField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorIntFieldStyle.subStyles[GUIIntField::getInputStyleType()] = GUIInputBox::getGUITypeName();

		skin->setStyle(GUIIntField::getGUITypeName(), editorIntFieldStyle);

		GUIElementStyle editorFloatFieldStyle;
		editorFloatFieldStyle.fixedHeight = true;
		editorFloatFieldStyle.height = 21;
		editorFloatFieldStyle.minWidth = 30;
		editorFloatFieldStyle.subStyles[GUIFloatField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorFloatFieldStyle.subStyles[GUIFloatField::getInputStyleType()] = GUIInputBox::getGUITypeName();

		skin->setStyle(GUIFloatField::getGUITypeName(), editorFloatFieldStyle);

		GUIElementStyle editorTextFieldStyle;
		editorTextFieldStyle.fixedHeight = true;
		editorTextFieldStyle.height = 21;
		editorTextFieldStyle.minWidth = 30;
		editorTextFieldStyle.subStyles[GUITextField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorTextFieldStyle.subStyles[GUITextField::getInputStyleType()] = GUIInputBox::getGUITypeName();

		skin->setStyle(GUITextField::getGUITypeName(), editorTextFieldStyle);

		GUIElementStyle editorColorFieldStyle;
		editorColorFieldStyle.fixedHeight = true;
		editorColorFieldStyle.height = 15;
		editorColorFieldStyle.minWidth = 30;
		editorColorFieldStyle.subStyles[GUIColorField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorColorFieldStyle.subStyles[GUIColorField::getColorInputStyleType()] = GUIColor::getGUITypeName();

		skin->setStyle(GUIColorField::getGUITypeName(), editorColorFieldStyle);

		GUIElementStyle editorToggleFieldStyle;
		editorToggleFieldStyle.fixedHeight = true;
		editorToggleFieldStyle.height = 15;
		editorToggleFieldStyle.minWidth = 30;
		editorToggleFieldStyle.subStyles[GUIToggleField::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorToggleFieldStyle.subStyles[GUIToggleField::getToggleStyleType()] = GUIToggle::getGUITypeName();

		skin->setStyle(GUIToggleField::getGUITypeName(), editorToggleFieldStyle);

		GUIElementStyle editorVector2FieldStyle;
		editorVector2FieldStyle.fixedHeight = true;
		editorVector2FieldStyle.height = 30;
		editorVector2FieldStyle.minWidth = 30;
		editorVector2FieldStyle.subStyles[GUIVector2Field::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorVector2FieldStyle.subStyles[GUIVector2Field::getFloatFieldStyleType()] = GUIFloatField::getGUITypeName();

		skin->setStyle(GUIVector2Field::getGUITypeName(), editorVector2FieldStyle);

		GUIElementStyle editorVector3FieldStyle;
		editorVector3FieldStyle.fixedHeight = true;
		editorVector3FieldStyle.height = 30;
		editorVector3FieldStyle.minWidth = 30;
		editorVector3FieldStyle.subStyles[GUIVector3Field::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorVector3FieldStyle.subStyles[GUIVector3Field::getFloatFieldStyleType()] = GUIFloatField::getGUITypeName();

		skin->setStyle(GUIVector3Field::getGUITypeName(), editorVector3FieldStyle);

		GUIElementStyle editorVector4FieldStyle;
		editorVector4FieldStyle.fixedHeight = true;
		editorVector4FieldStyle.height = 30;
		editorVector4FieldStyle.minWidth = 30;
		editorVector4FieldStyle.subStyles[GUIVector4Field::getLabelStyleType()] = GUITextField::getLabelStyleType();
		editorVector4FieldStyle.subStyles[GUIVector4Field::getFloatFieldStyleType()] = GUIFloatField::getGUITypeName();

		skin->setStyle(GUIVector4Field::getGUITypeName(), editorVector4FieldStyle);

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
		cmpFoldoutBtnStyle.normal.textColor = TextNormalColor;
		cmpFoldoutBtnStyle.hover.textColor = TextNormalColor;
		cmpFoldoutBtnStyle.active.textColor = TextNormalColor;
		cmpFoldoutBtnStyle.normalOn.textColor = TextNormalColor;
		cmpFoldoutBtnStyle.hoverOn.textColor = TextNormalColor;
		cmpFoldoutBtnStyle.activeOn.textColor = TextNormalColor;
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

		skin->setStyle(GUIComponentFoldout::getFoldoutButtonStyleType(), cmpFoldoutBtnStyle);

		GUIElementStyle cmpFoldoutRemoveBtnStyle;
		cmpFoldoutRemoveBtnStyle.normal.texture = getGUITexture(XButtonNormalTex);
		cmpFoldoutRemoveBtnStyle.hover.texture = getGUITexture(XButtonHoverTex);
		cmpFoldoutRemoveBtnStyle.active.texture = cmpFoldoutRemoveBtnStyle.hover.texture;
		cmpFoldoutRemoveBtnStyle.fixedHeight = true;
		cmpFoldoutRemoveBtnStyle.fixedWidth = true;
		cmpFoldoutRemoveBtnStyle.height = 11;
		cmpFoldoutRemoveBtnStyle.width = 11;

		skin->setStyle(GUIComponentFoldout::getFoldoutRemoveButtonStyleType(), cmpFoldoutRemoveBtnStyle);

		GUIElementStyle cmpFoldoutStyle;
		cmpFoldoutStyle.fixedHeight = true;
		cmpFoldoutStyle.height = 12;
		cmpFoldoutStyle.minWidth = 30;
		cmpFoldoutStyle.subStyles[GUIComponentFoldout::getFoldoutButtonStyleType()] = GUIComponentFoldout::getFoldoutButtonStyleType();
		cmpFoldoutStyle.subStyles[GUIComponentFoldout::getFoldoutRemoveButtonStyleType()] = GUIComponentFoldout::getFoldoutRemoveButtonStyleType();

		skin->setStyle(GUIComponentFoldout::getGUITypeName(), cmpFoldoutStyle);

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

		skin->setStyle(GUIFoldout::getFoldoutButtonStyleType(), foldoutBtnStyle);

		GUIElementStyle foldoutStyle;
		foldoutStyle.fixedHeight = true;
		foldoutStyle.height = 12;
		foldoutStyle.minWidth = 30;
		foldoutStyle.subStyles[GUIFoldout::getLabelStyleType()] = GUIFoldout::getLabelStyleType();
		foldoutStyle.subStyles[GUIFoldout::getFoldoutButtonStyleType()] = GUIFoldout::getFoldoutButtonStyleType();

		skin->setStyle(GUIFoldout::getGUITypeName(), foldoutStyle);

		/************************************************************************/
		/* 								PROGRESS BAR                      		*/
		/************************************************************************/
		GUIElementStyle progressBarBgStyle;
		progressBarBgStyle.fixedHeight = true;
		progressBarBgStyle.height = 18;
		progressBarBgStyle.normal.texture = getGUITexture(ProgressBarBgTex);

		skin->setStyle(GUIProgressBar::getBackgroundStyleType(), progressBarBgStyle);

		GUIElementStyle progressBarFillStyle;
		progressBarFillStyle.fixedHeight = true;
		progressBarFillStyle.height = 12;
		progressBarFillStyle.normal.texture = getGUITexture(ProgressBarFillTex);

		skin->setStyle(GUIProgressBar::getBarStyleType(), progressBarFillStyle);

		GUIElementStyle progressBarStyle;
		progressBarStyle.fixedHeight = true;
		progressBarStyle.height = 18;
		progressBarStyle.minWidth = 100;
		progressBarStyle.margins.top = 3;
		progressBarStyle.margins.bottom = 3;

		progressBarStyle.subStyles[GUIProgressBar::getBarStyleType()] = GUIProgressBar::getBarStyleType();
		progressBarStyle.subStyles[GUIProgressBar::getBackgroundStyleType()] = GUIProgressBar::getBackgroundStyleType();

		skin->setStyle(GUIProgressBar::getGUITypeName(), progressBarStyle);

		/************************************************************************/
		/* 							COLOR PICKER SLIDER                      	*/
		/************************************************************************/

		GUIElementStyle colorPickerSliderHorzHandleStyle;
		colorPickerSliderHorzHandleStyle.fixedHeight = true;
		colorPickerSliderHorzHandleStyle.fixedWidth = true;
		colorPickerSliderHorzHandleStyle.height = 20;
		colorPickerSliderHorzHandleStyle.width = 7;
		colorPickerSliderHorzHandleStyle.normal.texture = getGUITexture(ColorPickerSliderHorzHandleTex);
		colorPickerSliderHorzHandleStyle.hover.texture = colorPickerSliderHorzHandleStyle.normal.texture;
		colorPickerSliderHorzHandleStyle.active.texture = colorPickerSliderHorzHandleStyle.normal.texture;

		skin->setStyle("ColorSliderHorzHandle", colorPickerSliderHorzHandleStyle);

		GUIElementStyle colorPickerSliderHorzStyle;
		colorPickerSliderHorzHandleStyle.fixedHeight = true;
		colorPickerSliderHorzHandleStyle.height = 32;
		colorPickerSliderHorzHandleStyle.minWidth = 20;
		colorPickerSliderHorzStyle.subStyles[GUISlider::getHandleStyleType()] = "ColorSliderHorzHandle";

		skin->setStyle("ColorSliderHorz", colorPickerSliderHorzStyle);

		GUIElementStyle colorPickerSliderVertHandleStyle;
		colorPickerSliderVertHandleStyle.fixedHeight = true;
		colorPickerSliderVertHandleStyle.fixedWidth = true;
		colorPickerSliderVertHandleStyle.height = 7;
		colorPickerSliderVertHandleStyle.width = 45;
		colorPickerSliderVertHandleStyle.normal.texture = getGUITexture(ColorPickerSliderVertHandleTex);
		colorPickerSliderVertHandleStyle.hover.texture = colorPickerSliderVertHandleStyle.normal.texture;
		colorPickerSliderVertHandleStyle.active.texture = colorPickerSliderVertHandleStyle.normal.texture;

		skin->setStyle("ColorSliderVertHandle", colorPickerSliderVertHandleStyle);

		GUIElementStyle colorPickerSliderVertStyle;
		colorPickerSliderVertStyle.fixedWidth = true;
		colorPickerSliderVertStyle.width = 30;
		colorPickerSliderVertStyle.minHeight = 20;
		colorPickerSliderVertStyle.subStyles[GUISlider::getHandleStyleType()] = "ColorSliderVertHandle";

		skin->setStyle("ColorSliderVert", colorPickerSliderVertStyle);

		GUIElementStyle colorPickerSlider2DHandleStyle;
		colorPickerSlider2DHandleStyle.fixedHeight = true;
		colorPickerSlider2DHandleStyle.fixedWidth = true;
		colorPickerSlider2DHandleStyle.height = 8;
		colorPickerSlider2DHandleStyle.width = 8;
		colorPickerSlider2DHandleStyle.normal.texture = getGUITexture(ColorPickerSlider2DHandleTex);
		colorPickerSlider2DHandleStyle.hover.texture = colorPickerSlider2DHandleStyle.normal.texture;
		colorPickerSlider2DHandleStyle.active.texture = colorPickerSlider2DHandleStyle.normal.texture;

		skin->setStyle("ColorSlider2DHandle", colorPickerSlider2DHandleStyle);

		/************************************************************************/
		/* 								STATUS BAR                      		*/
		/************************************************************************/
		GUIElementStyle statusBarBgStyle;
		statusBarBgStyle.fixedHeight = true;
		statusBarBgStyle.height = 13;
		statusBarBgStyle.normal.texture = getGUITexture(StatusBarBgTex);

		skin->setStyle(GUIStatusBar::getGUIBackgroundTypeName(), statusBarBgStyle);

		GUIElementStyle statusBarMessageBtnStyle;
		statusBarMessageBtnStyle.font = font;
		statusBarMessageBtnStyle.fontSize = DefaultFontSize;
		statusBarMessageBtnStyle.fixedWidth = false;
		statusBarMessageBtnStyle.fixedHeight = true;
		statusBarMessageBtnStyle.height = 13;
		statusBarMessageBtnStyle.minWidth = 10;
		statusBarMessageBtnStyle.textHorzAlign = THA_Left;
		statusBarMessageBtnStyle.textVertAlign = TVA_Center;
		statusBarMessageBtnStyle.imagePosition = GUIImagePosition::Left;

		skin->setStyle(GUIStatusBar::getGUIMessageTypeName(), statusBarMessageBtnStyle);

		GUIElementStyle statusBarStyle;
		statusBarStyle.fixedHeight = true;
		statusBarStyle.height = 13;

		statusBarStyle.subStyles[GUIStatusBar::getGUIBackgroundTypeName()] = GUIStatusBar::getGUIBackgroundTypeName();
		statusBarStyle.subStyles[GUIStatusBar::getGUIMessageTypeName()] = GUIStatusBar::getGUIMessageTypeName();

		skin->setStyle(GUIStatusBar::getGUITypeName(), statusBarStyle);

		/************************************************************************/
		/* 									OTHER                      			*/
		/************************************************************************/

		// Right-aligned label
		GUIElementStyle rightAlignedLabelStyle;
		rightAlignedLabelStyle.font = font;
		rightAlignedLabelStyle.fontSize = DefaultFontSize;
		rightAlignedLabelStyle.fixedWidth = false;
		rightAlignedLabelStyle.fixedHeight = true;
		rightAlignedLabelStyle.height = 11;
		rightAlignedLabelStyle.minWidth = 10;
		rightAlignedLabelStyle.textHorzAlign = THA_Right;
		rightAlignedLabelStyle.normal.textColor = TextNormalColor;

		skin->setStyle("RightAlignedLabel", rightAlignedLabelStyle);

		// Multi-line label
		GUIElementStyle multiLinelabelStyle;
		multiLinelabelStyle.font = font;
		multiLinelabelStyle.fontSize = DefaultFontSize;
		multiLinelabelStyle.fixedWidth = false;
		multiLinelabelStyle.fixedHeight = true;
		multiLinelabelStyle.height = 11;
		multiLinelabelStyle.minWidth = 10;
		multiLinelabelStyle.wordWrap = true;
		multiLinelabelStyle.normal.textColor = TextNormalColor;

		skin->setStyle("MultiLineLabel", multiLinelabelStyle);

		// Multi-line centered label
		GUIElementStyle multiLineCenteredLabelStyle;
		multiLineCenteredLabelStyle.font = font;
		multiLineCenteredLabelStyle.fontSize = DefaultFontSize;
		multiLineCenteredLabelStyle.fixedWidth = false;
		multiLineCenteredLabelStyle.fixedHeight = true;
		multiLineCenteredLabelStyle.height = 11;
		multiLineCenteredLabelStyle.minWidth = 10;
		multiLineCenteredLabelStyle.wordWrap = true;
		multiLineCenteredLabelStyle.textHorzAlign = THA_Center;
		multiLineCenteredLabelStyle.normal.textColor = TextNormalColor;

		skin->setStyle("MultiLineLabelCentered", multiLineCenteredLabelStyle);

		// Selection area
		GUIElementStyle selectionAreaStyle;
		selectionAreaStyle.normal.texture = getGUITexture(SelectionAreaTex);
		selectionAreaStyle.border.left = 1;
		selectionAreaStyle.border.right = 1;
		selectionAreaStyle.border.top = 1;
		selectionAreaStyle.border.bottom = 1;

		skin->setStyle("SelectionArea", selectionAreaStyle);

		// Selectable label
		GUIElementStyle selectableLabelStyle;
		selectableLabelStyle.normalOn.texture = getGUITexture(SelectionBgTex);
		selectableLabelStyle.hoverOn.texture = selectableLabelStyle.normalOn.texture;
		selectableLabelStyle.activeOn.texture = selectableLabelStyle.normalOn.texture;
		selectableLabelStyle.fixedHeight = true;
		selectableLabelStyle.height = 11;
		selectableLabelStyle.minWidth = 10;
		selectableLabelStyle.font = font;
		selectableLabelStyle.fontSize = DefaultFontSize;
		selectableLabelStyle.textHorzAlign = THA_Left;
		selectableLabelStyle.normal.textColor = TextNormalColor;

		skin->setStyle("SelectableLabel", selectableLabelStyle);

		// Scroll area background
		GUIElementStyle scrollAreaBg;
		scrollAreaBg.normal.texture = getGUITexture(ScrollAreaBgTex);
		scrollAreaBg.minHeight = 6;
		scrollAreaBg.minWidth = 4;
		scrollAreaBg.border.left = 2;
		scrollAreaBg.border.right = 2;
		scrollAreaBg.border.top = 2;
		scrollAreaBg.border.bottom = 4;
		
		skin->setStyle("ScrollAreaBg", scrollAreaBg);

		return skin;
	}

	HSpriteTexture BuiltinEditorResources::getGUITexture(const WString& name)
	{
		Path texturePath = FileSystem::getWorkingDirectoryPath();
		texturePath.append(EditorSkinFolder);
		texturePath.append(L"sprite_" + name + L".asset");

		return Resources::instance().load<SpriteTexture>(texturePath);
	}

	HSpriteTexture BuiltinEditorResources::getGUIIcon(const WString& name)
	{
		Path texturePath = FileSystem::getWorkingDirectoryPath();
		texturePath.append(EditorIconFolder);
		texturePath.append(L"sprite_" + name + L".asset");

		return Resources::instance().load<SpriteTexture>(texturePath);
	}

	HShader BuiltinEditorResources::getShader(const WString& name)
	{
		Path programPath = EditorShaderFolder;
		programPath.append(name + L".asset");

		return gResources().load<Shader>(programPath);
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

	HMaterial BuiltinEditorResources::createWireHandleMat() const
	{
		return Material::create(mShaderHandleWire);
	}

	HMaterial BuiltinEditorResources::createSolidHandleMat() const
	{
		return Material::create(mShaderHandleSolid);
	}

	HMaterial BuiltinEditorResources::createSelectionMat() const
	{
		return Material::create(mShaderSelection);
	}

	HSpriteTexture BuiltinEditorResources::getLibraryIcon(ProjectIcon icon) const
	{
		switch (icon)
		{
		case ProjectIcon::Folder:
			return getGUIIcon(FolderIconTex);
		case ProjectIcon::Font:
			return getGUIIcon(FontIconTex);
		case ProjectIcon::Mesh:
			return getGUIIcon(MeshIconTex);
		case ProjectIcon::Texture:
			return getGUIIcon(TextureIconTex);
		case ProjectIcon::PlainText:
			return getGUIIcon(PlainTextIconTex);
		case ProjectIcon::ScriptCode:
			return getGUIIcon(ScriptCodeIconTex);
		case ProjectIcon::Shader:
			return getGUIIcon(ShaderIconTex);
		case ProjectIcon::ShaderInclude:
			return getGUIIcon(ShaderIncludeIconTex);
		case ProjectIcon::Material:
			return getGUIIcon(MaterialIconTex);
		case ProjectIcon::SpriteTexture:
			return getGUIIcon(SpriteTextureIconTex);
		case ProjectIcon::Prefab:
			return getGUIIcon(PrefabIconTex);
		}

		return HSpriteTexture();
	}

	HSpriteTexture BuiltinEditorResources::getLogMessageIcon(LogMessageIcon icon) const
	{
		switch (icon)
		{
		case LogMessageIcon::Info:
			return getGUIIcon(LogInfoIconTex);
		case LogMessageIcon::Warning:
			return getGUIIcon(LogWarningIconTex);
		case LogMessageIcon::Error:
			return getGUIIcon(LogErrorIconTex);
		}

		return HSpriteTexture();
	}
}