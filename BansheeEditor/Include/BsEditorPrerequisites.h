#pragma once

#include "BsPrerequisites.h"

#if (CM_PLATFORM == CM_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_ED_EXPORTS
#		define BS_ED_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_ED_EXPORT
#       else
#    		define BS_ED_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( CM_GCC_VISIBILITY )
#    define BS_ED_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_ED_EXPORT
#endif

namespace BansheeEditor
{
	// GUI
	class EditorWindowBase;
	class EditorWindow;
	class EditorWidgetBase;
	class EditorWidgetContainer;
	class EditorWidgetManager;
	class EditorWidgetLayout;
	class GUITabbedTitleBar;
	class GUITabButton;
	class GUIWindowDropArea;
	class GUIWindowFrame;
	class GUIIntField;
	class GUIFloatField;
	class GUITextField;
	class GUIColor;
	class GUIDropButton;
	class EditorWindowManager;
	class DockManager;
	class DockManagerLayout;
	class MainEditorWindow;
	class WindowFrameWidget;
	class GUIMenuBar;
	class GUIDockSlider;
	class GUISceneTreeView;
	class GUIResourceTreeView;
	class GUITreeViewEditBox;
	class EditorCommand;
	class ResourceMeta;

	typedef std::shared_ptr<ResourceMeta> ResourceMetaPtr;
	typedef std::shared_ptr<DockManagerLayout> DockManagerLayoutPtr;
	typedef std::shared_ptr<EditorWidgetLayout> EditorWidgetLayoutPtr;

	enum class DragAndDropType
	{
		EditorWidget = 10000,
		SceneObject = 10001,
		Resources = 10002
	};

	enum TypeID_BansheeEditor
	{
		TID_ResourceMeta = 40000,
		TID_ProjectLibraryEntries = 40001,
		TID_ProjectLibraryResEntry = 40002,
		TID_ProjectLibraryDirEntry = 40003,
		TID_DockManagerLayout = 40004,
		TID_DockManagerLayoutEntry = 40005,
		TID_EditorWidgetLayout = 40006,
		TID_EditorWidgetLayoutEntry = 40007
	};
}
