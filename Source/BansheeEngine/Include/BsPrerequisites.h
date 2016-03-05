//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsCorePrerequisites.h"

/** @defgroup Engine Engine
 *	Specific implementation of the BansheeCore layer. This layer provides a more focused goal than the more generic
 *  BansheeCore layer, and therefore provides specific implementations of various abstract interfaces from BansheeCore,
 *  either directly or through specific plugins.
 *  @{
 */

/** @defgroup Renderer-Engine Renderer
  *	Abstract interface and helper functionality for rendering scene objects and other geometry (e.g. GUI). Provides more
  * specialized functionality than Renderer abstraction in BansheeCore.
  */

/** @defgroup 2D 2D
  *	Functionality relating to two dimensional geometry like sprites.
  */

/** @defgroup Components Components
  *	Built-in components (elements that may be attached to scene objects).
  */

/** @defgroup GUI GUI
  *	Everything relating to the graphical user interface, including elements, styles, events and GUI manager.
  */

/** @defgroup Input-Engine Input
 *	Functionality for dealing with input (mouse, keyboard, gamepad, etc.).
 */

/** @defgroup RTTI-Impl-Engine RTTI types
 *  Types containing RTTI for specific classes.
 */

/** @defgroup Resources-Engine Resources
  *	Contains engine resource types and manager for builtin resources.
  */

/** @defgroup Utility-Engine Utility
 *  Various utility methods and types used by the engine layer.
 */

/** @defgroup Platform-Engine Platform
 *  Platform specific functionality.
 */

/** @defgroup Script Script
 *  Functionality for dealing with scripting languages and libraries.
 */

/** @defgroup Application-Engine Application
 *  Entry point into the application.
 */

/** @} */

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
#include "BsPaths.h"

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
	class CGUIWidget;
	class GUIManager;
	class GUIWidget;
	class GUIElementBase;
	class GUIElement;
	class GUILabel;
	class GUIButtonBase;
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

	class RenderableHandler;
	class ProfilerOverlay;
	class ProfilerOverlayInternal;
	class DrawHelper;
	class Camera;
	class Renderable;
	class CameraCore;
	class RenderableCore;
	class PlainText;
	class ScriptCode;
	class ScriptCodeImportOptions;
	class RendererMeshData;

	// 2D
	class TextSprite;
	class ImageSprite;
	class SpriteTexture;
	struct SpriteMaterialInfo;

	// Components
	class CRenderable;
	class CCamera;
	class CLight;

	typedef std::shared_ptr<TextSprite> TextSpritePtr;
	typedef std::shared_ptr<SpriteTexture> SpriteTexturePtr;
	typedef std::shared_ptr<CCamera> CCameraPtr;
	typedef std::shared_ptr<CRenderable> CRenderablePtr;
	typedef std::shared_ptr<GUIToggleGroup> GUIToggleGroupPtr;
	typedef std::shared_ptr<Camera> CameraPtr;
	typedef std::shared_ptr<Renderable> RenderablePtr;
	typedef std::shared_ptr<InputConfiguration> InputConfigurationPtr;
	typedef std::shared_ptr<PlainText> PlainTextPtr;
	typedef std::shared_ptr<ScriptCode> ScriptCodePtr;
	typedef std::shared_ptr<GUISkin> GUISkinPtr;
	typedef std::shared_ptr<GUIContextMenu> GUIContextMenuPtr;

	typedef GameObjectHandle<CGUIWidget> HGUIWidget;
	typedef GameObjectHandle<CCamera> HCamera;
	typedef GameObjectHandle<CRenderable> HRenderable;
	typedef GameObjectHandle<ProfilerOverlay> HProfilerOverlay;

	typedef ResourceHandle<SpriteTexture> HSpriteTexture;
	typedef ResourceHandle<PlainText> HPlainText;
	typedef ResourceHandle<ScriptCode> HScriptCode;
	typedef ResourceHandle<GUISkin> HGUISkin;

	/**
	 * @brief	RTTI types.
	 */
	enum TypeID_Banshee
	{
		TID_CCamera = 30000,
		TID_CRenderable = 30001,
		TID_SpriteTexture = 30002,
		TID_Camera = 30003,
		TID_Renderable = 30004,
		TID_PlainText = 30005,
		TID_ScriptCode = 30006,
		TID_ScriptCodeImportOptions = 30007,
		TID_GUIElementStyle = 30008,
		TID_GUISkin = 30009,
		TID_GUISkinEntry = 30010,
		TID_Light = 30011,
		TID_CLight = 30012,
		TID_GameSettings = 30013,
		TID_ResourceMapping = 30014
	};
}