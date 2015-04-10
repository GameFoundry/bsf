#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUISkin.h"
#include "BsModule.h"
#include "BsApplication.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT BuiltinEditorResources : public BansheeEngine::Module<BuiltinEditorResources>
	{
	public:
		BuiltinEditorResources(RenderSystemPlugin activeRSPlugin);

		const GUISkin& getSkin() const { return mSkin; }

		/**
		 * @brief	Creates a material used for docking drop overlay used by the editor.
		 */
		HMaterial createDockDropOverlayMaterial() const;

		/**
		 * @brief	Creates a material used for rendering the scene grid.
		 */
		HMaterial createSceneGridMaterial() const;

		/**
		 * @brief	Creates a material used for picking non-transparent objects in scene view.
		 */
		HMaterial createPicking(CullingMode cullMode) const;

		/**
		 * @brief	Creates a material used for picking transparent objects in scene view.
		 */
		HMaterial createPickingAlpha(CullingMode cullMode) const;

		/**
		 * @brief	Creates a material used for rendering wireframe gizmos.
		 */
		HMaterial createWireGizmoMat() const;

		/**
		 * @brief	Creates a material used for rendering solid gizmos.
		 */
		HMaterial createSolidGizmoMat() const;

		/**
		 * @brief	Creates a material used for rendering icon gizmos.
		 */
		HMaterial createIconGizmoMat() const;

		/**
		 * @brief	Creates a material used for picking non-transparent gizmos.
		 */
		HMaterial createGizmoPickingMat() const;

		/**
		 * @brief	Creates a material used for picking transparent gizmos.
		 */
		HMaterial createAlphaGizmoPickingMat() const;

		/**
		 * @brief	Creates a material used for rendering wireframe handles.
		 */
		HMaterial createWireHandleMat() const;

		/**
		 * @brief	Creates a material used for rendering solid handles.
		 */
		HMaterial createSolidHandleMat() const;

		/**
		 * @brief	Creates a material used for displaying selected objects.
		 */
		HMaterial createSelectionMat() const;

		static const String ObjectFieldStyleName;
		static const String ObjectFieldLabelStyleName;
		static const String ObjectFieldDropBtnStyleName;
		static const String ObjectFieldClearBtnStyleName;

	private:
		/**
		 * @brief	Imports all necessary resources and converts them to engine-ready format.
		 *
		 * @note	Normally you only want to use this during development phase and then ship
		 *			with engine-ready format only.
		 */
		void preprocess();

		/**
		 * @brief	Loads a GUI skin texture with the specified filename.
		 */
		static HSpriteTexture getGUITexture(const WString& name);

		/**
		 * @brief	Loads a GPU program with the specified filename.
		 */
		HGpuProgram getGpuProgram(const WString& name);

		/**
		 * @brief	Imports a GUI skin texture with the specified filename.
		 *			Saves the imported texture in engine-ready format in the corresponding
		 *			output folder.
		 */
		static void importGUITexture(const WString& name);

		/**
		 * @brief	Loads and compiles a shader for dock overlay rendering.
		 */
		void initDockDropOverlayShader();

		/**
		 * @brief	Loads and compiles a shader for scene grid rendering.
		 */
		void initSceneGridShader();

		/**
		 * @brief	Loads and compiles a shader used for scene picking (non alpha).
		 */
		void initPickingShader(CullingMode cullMode);

		/**
		 * @brief	Loads and compiles a shader used for scene picking (alpha).
		 */
		void initPickingAlphaShader(CullingMode cullMode);

		/**
		 * @brief	Loads and compiles a shader used for rendering wireframe gizmos.
		 */
		void initWireGizmoShader();

		/**
		 * @brief	Loads and compiles a shader used for rendering solid gizmos.
		 */
		void initSolidGizmoShader();

		/**
		 * @brief	Loads and compiles a shader used for rendering icon gizmos.
		 */
		void initIconGizmoShader();

		/**
		 * @brief	Loads and compiles a shader used for scene picking of non-transparent gizmos.
		 */
		void initGizmoPickingShader();

		/**
		 * @brief	Loads and compiles a shader used for scene picking of possibly transparent gizmos.
		 */
		void initGizmoPickingAlphaShader();

		/**
		 * @brief	Loads and compiles a shader used for rendering wireframe handles.
		 */
		void initWireHandleShader();

		/**
		 * @brief	Loads and compiles a shader used for rendering solid handles.
		 */
		void initSolidHandleShader();

		/**
		 * @brief	Loads and compiles a shader used for displaying selected objects.
		 */
		void initSelectionShader();

		RenderSystemPlugin mRenderSystemPlugin;
		WString mActiveShaderSubFolder;
		StringID mActiveRenderSystem;

		HShader mShaderDockOverlay;
		HShader mShaderSceneGrid;
		HShader mShaderPicking[3];
		HShader mShaderPickingAlpha[3];
		HShader mShaderGizmoSolid;
		HShader mShaderGizmoWire;
		HShader mShaderGizmoIcon;
		HShader mShaderGizmoPicking;
		HShader mShaderGizmoAlphaPicking;
		HShader mShaderHandleSolid;
		HShader mShaderHandleWire;
		HShader mShaderSelection;

		GUISkin mSkin;

		static const Path DefaultSkinFolder;
		static const Path DefaultSkinFolderRaw;

		static const Path DefaultShaderFolder;
		static const Path DefaultShaderFolderRaw;

		static const WString HLSL11ShaderSubFolder;
		static const WString HLSL9ShaderSubFolder;
		static const WString GLSLShaderSubFolder;

		static const WString DefaultFontFilename;
		static const UINT32 DefaultFontSize;

		static const WString WindowBackgroundTexture;

		static const WString WindowFrameNormal;
		static const WString WindowFrameFocused;

		static const WString WindowTitleBarBg;

		static const WString WindowCloseButtonNormal;
		static const WString WindowCloseButtonHover;

		static const WString WindowMinButtonNormal;
		static const WString WindowMinButtonHover;

		static const WString WindowMaxButtonNormal;
		static const WString WindowMaxButtonHover;

		static const WString TabbedBarBtnNormal;
		static const WString TabbedBarBtnActive;

		static const WString ButtonNormalTex;
		static const WString ButtonHoverTex;
		static const WString ButtonActiveTex;

		static const WString ToggleNormalTex;
		static const WString ToggleHoverTex;
		static const WString ToggleActiveTex;
		static const WString ToggleNormalOnTex;
		static const WString ToggleHoverOnTex;
		static const WString ToggleActiveOnTex;

		static const WString ObjectDropBtnNormalTex;
		static const WString ObjectDropBtnNormalOnTex;
		static const WString ObjectClearBtnNormalTex;
		static const WString ObjectClearBtnHoverTex;
		static const WString ObjectClearBtnActiveTex;

		static const WString FoldoutOpenNormalTex;
		static const WString FoldoutOpenHoverTex;
		static const WString FoldoutClosedNormalTex;
		static const WString FoldoutClosedHoverTex;

		static const WString CmpFoldoutOpenNormalTex;
		static const WString CmpFoldoutOpenHoverTex;
		static const WString CmpFoldoutOpenActiveTex;
		static const WString CmpFoldoutClosedNormalTex;
		static const WString CmpFoldoutClosedHoverTex;
		static const WString CmpFoldoutClosedActiveTex;

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

		static const WString MenuBarBgTex;

		static const WString MenuBarBtnNormalTex;
		static const WString MenuBarBtnHoverTex;

		static const WString MenuBarBansheeLogoTex;

		static const WString DockSliderNormalTex;

		static const WString TreeViewExpandButtonOffNormal;
		static const WString TreeViewExpandButtonOffHover;
		static const WString TreeViewExpandButtonOnNormal;
		static const WString TreeViewExpandButtonOnHover;

		static const WString TreeViewSelectionBackground;
		static const WString TreeViewEditBox;

		static const WString TreeViewElementHighlight;
		static const WString TreeViewElementSepHighlight;

		static const WString ColorPickerSliderHorzHandleTex;
		static const WString ColorPickerSliderVertHandleTex;
		static const WString ColorPickerSlider2DHandleTex;

		static const WString ProgressBarFillTex;
		static const WString ProgressBarBgTex;

		static const WString ShaderDockOverlayVSFile;
		static const WString ShaderDockOverlayPSFile;
		static const WString SceneGridVSFile;
		static const WString SceneGridPSFile;
		static const WString PickingVSFile;
		static const WString PickingPSFile;
		static const WString PickingAlphaVSFile;
		static const WString PickingAlphaPSFile;
		static const WString ShaderWireGizmoVSFile;
		static const WString ShaderWireGizmoPSFile;
		static const WString ShaderSolidGizmoVSFile;
		static const WString ShaderSolidGizmoPSFile;
		static const WString ShaderIconGizmo0VSFile;
		static const WString ShaderIconGizmo0PSFile;
		static const WString ShaderIconGizmo1VSFile;
		static const WString ShaderIconGizmo1PSFile;
		static const WString GizmoPickingVSFile;
		static const WString GizmoPickingPSFile;
		static const WString GizmoPickingAlphaVSFile;
		static const WString GizmoPickingAlphaPSFile;
		static const WString SelectionVSFile;
		static const WString SelectionPSFile;
	};
}