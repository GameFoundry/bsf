#pragma once

#include "BsPrerequisites.h"

namespace BansheeEditor
{
	// GUI
	class EditorWindowBase;
	class EditorWindow;
	class EditorWidgetBase;
	class EditorWidgetContainer;
	class EditorWidgetManager;
	class GUITabbedTitleBar;
	class GUITabButton;
	class GUIWindowDropArea;
	class GUIWindowFrame;
	class EditorWindowManager;
	class DockManager;
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
		TID_ProjectLibraryDirEntry = 40003
	};
}
