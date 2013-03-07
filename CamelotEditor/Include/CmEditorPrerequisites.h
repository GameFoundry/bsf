#pragma once

#include "CmPrerequisites.h"

using namespace CamelotEngine; // Not great design, but I'm just using namespaces to make the distinction between engine and editor cleaner so its fine

namespace CamelotEditor
{
	class LayoutManager;
	class EditorPrefs;
	class EditorApplication;
	class ProjectPrefs;
	class QtEditor;
	class QtDockOverlayWidget;
	class EditorWidgetFactory;
	class QtEditorWindow;
	class QtEditorWidget;
	class QtDynamicTabBar;
}

// Qt
class QWidget;
class QLabel;
class QPushButton;
class QSplitter;
class QStackedWidget;
class QHBoxLayout;
class QVBoxLayout;