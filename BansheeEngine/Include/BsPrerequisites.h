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
	typedef char INT8;
	typedef unsigned char UINT8;
	typedef short INT16;
	typedef unsigned short UINT16;
	typedef int INT32;
	typedef unsigned int UINT32;
	typedef unsigned short WCHAR;

#if CM_COMPILER == CM_COMPILER_MSVC
	typedef unsigned __int64 UINT64;
	typedef __int64 INT64;
#else
	typedef unsigned long long UINT64;
	typedef long long INT64;
#endif

	// GUI
	class GUIManager;
	class GUIWidget;
	class GUIElement;
	class GUILabel;
	class GUISkin;
	struct GUIElementStyle;
	class GUIMouseEvent;

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