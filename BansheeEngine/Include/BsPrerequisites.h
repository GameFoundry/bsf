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

namespace BansheeEngine
{
	static const String RenderAPIDX9 = "D3D9RenderAPI";
	static const String RenderAPIDX11 = "D3D11RenderAPI";
	static const String RenderAPIOpenGL = "GLRenderAPI";
	static const String RendererDefault = "BansheeRenderer";
	static const String RendererInvariant = "CoreRenderer";

	class VirtualButton;
	class VirtualInput;
	class InputConfiguration;
	struct DragCallbackInfo;

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
	class GUIArea;
	class GUILayout;
	class GUILayoutX;
	class GUILayoutY;
	class GUILayoutExplicit;
	class GUIFixedSpace;
	class GUIFlexibleSpace;
	class GUIInputCaret;
	class GUIInputSelection;
	struct GUILayoutOptions;
	class GUIOptions;
	class GUIToggleGroup;
	class GUIListBox;
	class GUIDropDownDataEntry;
	class GUIDropDownBox;
	class DragAndDropManager;
	class GUIMenu;
	class GUIMenuItem;
	class GUIContent;
	class GUIContextMenu;
	class GUIDropDownHitBox;
	class RenderableElement;
	class GUISlider;
	class GUISliderVert;
	class GUISliderHorz;
	class GUIProgressBar;

	class RenderableController;
	class ProfilerOverlay;
	class DrawHelper;
	class CameraHandler;
	class RenderableHandler;
	class CameraHandlerCore;
	class RenderableHandlerCore;
	class PlainText;
	class ScriptCode;

	// 2D
	class TextSprite;
	class ImageSprite;
	class SpriteTexture;
	class OverlayManager;

	// Components
	class Overlay;
	class Renderable;
	class Camera;

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

	typedef GameObjectHandle<GUIWidget> HGUIWidget;
	typedef GameObjectHandle<Camera> HCamera;
	typedef GameObjectHandle<Renderable> HRenderable;
	typedef GameObjectHandle<ProfilerOverlay> HProfilerOverlay;

	typedef ResourceHandle<SpriteTexture> HSpriteTexture;
	typedef ResourceHandle<PlainText> HPlainText;
	typedef ResourceHandle<ScriptCode> HScriptCode;

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
		TID_ScriptCode = 30006
	};
}