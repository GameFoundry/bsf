#pragma once

#include "BsCorePrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_EXPORTS
#		define BS_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_EXPORT
#       else
#    		define BS_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_EXPORT
#endif

#include "BsGameObject.h"
#include "BsEnums.h"
#include "BsHEString.h"

namespace BansheeEngine
{
	static const StringID RenderAPIDX9 = "D3D9RenderAPI";
	static const StringID RenderAPIDX11 = "D3D11RenderAPI";
	static const StringID RenderAPIOpenGL = "GLRenderAPI";
	static const StringID RendererDefault = "RenderBeast";

	class VirtualButton;
	class VirtualInput;
	class InputConfiguration;
	struct DragCallbackInfo;
	struct ShortcutKey;

	// GUI
	class GUIManager;
	class GUIWidget;
	class GUIElementBase;
	class GUIElement;
	class GUILabel;
	class GUIButton;
	class GUITexture;
	class GUIToggle;
	class GUIInputBox;
	class GUISliderHandle;
	class GUIScrollBarVert;
	class GUIScrollBarHorz;
	class GUIScrollArea;
	class GUISkin;
	class GUIRenderTexture;
	struct GUIElementStyle;
	class GUIMouseEvent;
	class GUITextInputEvent;
	class GUICommandEvent;
	class GUIVirtualButtonEvent;
	class GUILayout;
	class GUILayoutX;
	class GUILayoutY;
	class GUIPanel;
	class GUIFixedSpace;
	class GUIFlexibleSpace;
	class GUIInputCaret;
	class GUIInputSelection;
	struct GUIDimensions;
	class GUIOptions;
	class GUIToggleGroup;
	class GUIListBox;
	class GUIDropDownDataEntry;
	class GUIDropDownMenu;
	class DragAndDropManager;
	class GUIMenu;
	class GUIMenuItem;
	class GUIContent;
	class GUIContextMenu;
	class GUIDropDownHitBox;
	class GUIDropDownContent;
	class RenderableElement;
	class GUISlider;
	class GUISliderVert;
	class GUISliderHorz;
	class GUIProgressBar;

	class RenderableController;
	class ProfilerOverlay;
	class ProfilerOverlayInternal;
	class DrawHelper;
	class CameraHandler;
	class RenderableHandler;
	class CameraHandlerCore;
	class RenderableHandlerCore;
	class PlainText;
	class ScriptCode;
	class ScriptCodeImportOptions;
	class RendererMeshData;

	// 2D
	class TextSprite;
	class ImageSprite;
	class SpriteTexture;
	class OverlayManager;

	// Components
	class Overlay;
	class Renderable;
	class Camera;
	class Light;

	typedef std::shared_ptr<TextSprite> TextSpritePtr;
	typedef std::shared_ptr<SpriteTexture> SpriteTexturePtr;
	typedef std::shared_ptr<Overlay> OverlayPtr;
	typedef std::shared_ptr<Camera> CameraPtr;
	typedef std::shared_ptr<Renderable> RenderablePtr;
	typedef std::shared_ptr<GUIToggleGroup> GUIToggleGroupPtr;
	typedef std::shared_ptr<CameraHandler> CameraHandlerPtr;
	typedef std::shared_ptr<RenderableHandler> RenderableHandlerPtr;
	typedef std::shared_ptr<InputConfiguration> InputConfigurationPtr;
	typedef std::shared_ptr<PlainText> PlainTextPtr;
	typedef std::shared_ptr<ScriptCode> ScriptCodePtr;
	typedef std::shared_ptr<GUISkin> GUISkinPtr;
	typedef std::shared_ptr<GUIContextMenu> GUIContextMenuPtr;

	typedef GameObjectHandle<GUIWidget> HGUIWidget;
	typedef GameObjectHandle<Camera> HCamera;
	typedef GameObjectHandle<Renderable> HRenderable;
	typedef GameObjectHandle<ProfilerOverlay> HProfilerOverlay;

	typedef ResourceHandle<SpriteTexture> HSpriteTexture;
	typedef ResourceHandle<PlainText> HPlainText;
	typedef ResourceHandle<ScriptCode> HScriptCode;
	typedef ResourceHandle<GUISkin> HGUISkin;

	static const char* ENGINE_ASSEMBLY = "MBansheeEngine";
	static const char* SCRIPT_GAME_ASSEMBLY = "MScriptGame";
	static const Path ASSEMBLY_PATH = "..\\..\\Assemblies\\";

	/**
	 * @brief	RTTI types.
	 */
	enum TypeID_Banshee
	{
		TID_Camera = 30000,
		TID_Renderable = 30001,
		TID_SpriteTexture = 30002,
		TID_CameraHandler = 30003,
		TID_RenderableHandler = 30004,
		TID_PlainText = 30005,
		TID_ScriptCode = 30006,
		TID_ScriptCodeImportOptions = 30007,
		TID_GUIElementStyle = 30008,
		TID_GUISkin = 30009,
		TID_GUISkinEntry = 30010,
		TID_LightInternal = 30011,
		TID_Light = 30012
	};
}