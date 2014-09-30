#pragma once

#include "BsPrerequisites.h"
#include "BsGUISkin.h"
#include "BsModule.h"
#include "BsPath.h"
#include "BsVector2I.h"
#include "BsGUIMaterialInfo.h"
#include "BsDebugDrawMaterialInfo.h"
#include "BsApplication.h"

namespace BansheeEngine
{
	/**
	 * @brief	Holds references to built-in resources used by the core engine.
	 */
	class BS_EXPORT BuiltinResources : public BansheeEngine::Module<BuiltinResources>
	{
	public:
		BuiltinResources(RenderSystemPlugin activeRSPlugin);
		~BuiltinResources();

		/**
		 * @brief	Returns the default skin used by engine GUI elements.
		 */
		const GUISkin& getGUISkin() const { return mSkin; }

		/**
		 * @brief	Returns a small entirely white texture.
		 */
		const HSpriteTexture getWhiteSpriteTexture() const { return mWhiteSpriteTexture; }

		/**
		 * @brief	Returns image data for an arrow cursor, along with its hotspot.
		 */
		const PixelData& getCursorArrow(Vector2I& hotSpot);

		/**
		 * @brief	Returns image data for an arrow with dragged object cursor, along with its hotspot.
		 */
		const PixelData& getCursorArrowDrag(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a wait cursor, along with its hotspot.
		 */
		const PixelData& getCursorWait(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for an "I" beam cursor, along with its hotspot.
		 */
		const PixelData& getCursorIBeam(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a NESW resize cursor, along with its hotspot.
		 */
		const PixelData& getCursorSizeNESW(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a NS resize cursor, along with its hotspot.
		 */
		const PixelData& getCursorSizeNS(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a NWSE resize cursor, along with its hotspot.
		 */
		const PixelData& getCursorSizeNWSE(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a WE resize cursor, along with its hotspot.
		 */
		const PixelData& getCursorSizeWE(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a deny cursor, along with its hotspot.
		 */
		const PixelData& getCursorDeny(Vector2I& hotSpot);
		
		/**
		 * @brief	Returns image data for a move left-right cursor, along with its hotspot.
		 */
		const PixelData& getCursorMoveLeftRight(Vector2I& hotSpot);

		/**
		 * @brief	Creates material used for textual sprite rendering (e.g. text in GUI).
		 */
		GUIMaterialInfo createSpriteTextMaterial() const;

		/**
		 * @brief	Creates material used for image sprite rendering (e.g. images in GUI).
		 */
		GUIMaterialInfo createSpriteImageMaterial() const;

		/**
		 * @brief	Creates a material used as a replacement when no other material is usable.
		 */
		HMaterial createDummyMaterial() const;

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
		static HSpriteTexture getSkinTexture(const WString& name);

		/**
		 * @brief	Loads a cursor texture with the specified filename.
		 */
		static HTexture getCursorTexture(const WString& name);

		/**
		 * @brief	Loads a GPU program with the specified filename.
		 */
		HGpuProgram getGpuProgram(const WString& name);

		/**
		 * @brief	Imports a GUI skin texture with the specified filename.
		 *			Saves the imported texture in engine-ready format in the corresponding
		 *			output folder.
		 */
		static void importSkinTexture(const WString& name);

		/**
		 * @brief	Imports a cursor texture with the specified filename.
		 *			Saves the imported texture in engine-ready format in the corresponding
		 *			output folder.
		 */
		static void importCursorTexture(const WString& name);

		/**
		 * @brief	Loads an compiles a shader for text rendering.
		 */
		void initSpriteTextShader();

		/**
		 * @brief	Loads an compiles a shader for image sprite rendering.
		 */
		void initSpriteImageShader();

		/**
		 * @brief	Loads an compiles a simple shader to be used with no other is usable.
		 */
		void initDummyShader();

		RenderSystemPlugin mRenderSystemPlugin;

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

		ShaderPtr mShaderSpriteText;
		ShaderPtr mShaderSpriteImage;
		ShaderPtr mShaderDummy;

		WString mActiveShaderSubFolder;
		String mActiveRenderSystem;

		static const Path DefaultSkinFolderRaw;
		static const Path DefaultCursorFolderRaw;
		static const Path DefaultShaderFolderRaw;

		static const Path DefaultSkinFolder;
		static const Path DefaultCursorFolder;
		static const Path DefaultShaderFolder;

		static const WString HLSL11ShaderSubFolder;
		static const WString HLSL9ShaderSubFolder;
		static const WString GLSLShaderSubFolder;

		static const WString DefaultFontFilename;
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

		static const WString ShaderSpriteTextVSFile;
		static const WString ShaderSpriteTextPSFile;
		static const WString ShaderSpriteImageVSFile;
		static const WString ShaderSpriteImagePSFile;
		static const WString ShaderDockOverlayVSFile;
		static const WString ShaderDockOverlayPSFile;
		static const WString ShaderDummyVSFile;
		static const WString ShaderDummyPSFile;
	};
}