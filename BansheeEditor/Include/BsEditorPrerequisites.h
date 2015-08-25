#pragma once

#include "BsPrerequisites.h"

#if (BS_PLATFORM == BS_PLATFORM_WIN32) && !defined(__MINGW32__)
#	ifdef BS_ED_EXPORTS
#		define BS_ED_EXPORT __declspec(dllexport)
#	else
#       if defined( __MINGW32__ )
#           define BS_ED_EXPORT
#       else
#    		define BS_ED_EXPORT __declspec(dllimport)
#       endif
#	endif
#elif defined ( BS_GCC_VISIBILITY )
#    define BS_ED_EXPORT  __attribute__ ((visibility("default")))
#else
#    define BS_ED_EXPORT
#endif

namespace BansheeEngine
{
	class EditorWindowBase;
	class EditorWindow;
	class EditorWidgetBase;
	class EditorWidgetContainer;
	class EditorWidgetManager;
	class EditorWidgetLayout;
	class ModalWindow;
	class GUITabbedTitleBar;
	class GUITabButton;
	class GUIWindowFrame;
	class GUIIntField;
	class GUIFloatField;
	class GUITextField;
	class GUIToggleField;
	class GUIVector2Field;
	class GUIVector3Field;
	class GUIVector4Field;
	class GUIColorField;
	class GUIColor;
	class GUIComponentFoldout;
	class GUIFoldout;
	class GUIStatusBar;
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
	class ProjectResourceMeta;
	class SceneGrid;
	class HandleSlider;
	class HandleSliderLine;
	class HandleSliderPlane;
	class HandleSliderDisc;
	class HandleSliderManager;
	class HandleDrawManager;
	class SceneCameraController;
	class EditorSettings;
	class SceneViewHandler;
	class SelectionRenderer;
	class DropDownWindow;
	class ProjectSettings;

	typedef std::shared_ptr<ProjectResourceMeta> ProjectResourceMetaPtr;
	typedef std::shared_ptr<DockManagerLayout> DockManagerLayoutPtr;
	typedef std::shared_ptr<EditorWidgetLayout> EditorWidgetLayoutPtr;
	typedef std::shared_ptr<EditorSettings> EditorSettingsPtr;
	typedef std::shared_ptr<ProjectSettings> ProjectSettingsPtr;

	static const char* EDITOR_ASSEMBLY = "MBansheeEditor";
	static const char* SCRIPT_EDITOR_ASSEMBLY = "MScriptEditor";
	static const Path PROJECT_INTERNAL_DIR = L"Internal\\";
	static const Path INTERNAL_ASSEMBLY_PATH = PROJECT_INTERNAL_DIR + "Assemblies\\";

	/**
	 * @brief	Types of drag and drop operations. Different types specify
	 *			different types of dragged data.
	 */
	enum class DragAndDropType
	{
		EditorWidget = 10000,
		SceneObject = 10001,
		Resources = 10002
	};

	/**
	 * @brief	Recognized types of external code editors.
	 *
	 * @see		CodeEditorManager
	 */
	enum class CodeEditorType
	{
		VS2008,
		VS2010,
		VS2012,
		VS2013,
		VS2015
	};

	/**
	 * @brief	Type IDs used by the RTTI system for the editor library.
	 */
	enum TypeID_BansheeEditor
	{
		TID_ProjectResourceMeta = 40000,
		TID_ProjectLibraryEntries = 40001,
		TID_ProjectLibraryResEntry = 40002,
		TID_ProjectLibraryDirEntry = 40003,
		TID_DockManagerLayout = 40004,
		TID_DockManagerLayoutEntry = 40005,
		TID_EditorWidgetLayout = 40006,
		TID_EditorWidgetLayoutEntry = 40007,
		TID_TestComponentA = 40008,
		TID_TestComponentB = 40009,
		TID_PlatformInfo = 40010,
		TID_WinPlatformInfo = 40011,
		TID_BuildData = 40012,
		TID_TestObjectA = 40013,
		TID_TestObjectB = 40014,
		TID_TestComponentC = 40015,
		TID_TestComponentD = 40016,
		TID_EditorSettings = 40017,
		TID_RecentProject = 40018,
		TID_Settings = 40019,
		TID_ProjectSettings = 40020
	};
}
