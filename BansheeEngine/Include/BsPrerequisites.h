#pragma once

#include "CmPrerequisites.h"

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_EXPORTS
#		define BS_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_EXPORT
#       else
#    		define BS_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define BS_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_EXPORT
#endif

#include "CmGameObject.h"

// To avoid a lot of typing
namespace CM = CamelotFramework;

namespace BansheeEngine
{
	// GUI
	class GUIManager;
	class GUIWidget;
	class GUIElement;
	class GUILabel;
	class GUIButton;
	class GUITexture;
	class GUIToggle;
	class GUIInputBox;
	class GUIScrollBarHandle;
	class GUIScrollBarVert;
	class GUIScrollBarHorz;
	class GUIScrollArea;
	class GUISkin;
	struct GUIElementStyle;
	class GUIMouseEvent;
	class GUIKeyEvent;
	class GUICommandEvent;
	class GUIArea;
	class GUILayout;
	class GUILayoutX;
	class GUILayoutY;
	class GUIFixedSpace;
	class GUIFlexibleSpace;
	class GUIInputCaret;
	class GUIInputSelection;
	struct GUILayoutOptions;
	class GUIToggleGroup;
	class GUIListBox;
	class GUIDropDownBox;
	class DragAndDropManager;
	class GUIContent;
	class GUIContextMenu;

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

	typedef CM::GameObjectHandle<GUIWidget> HGUIWidget;
	typedef CM::GameObjectHandle<Camera> HCamera;
	typedef CM::GameObjectHandle<Renderable> HRenderable;

	enum TypeID_Banshee
	{
		TID_Camera = 30000,
		TID_Renderable = 30001
	};
}

namespace BS = BansheeEngine;