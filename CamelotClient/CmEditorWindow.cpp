#include "CmEditorWindow.h"
#include "CmRenderWindow.h"
#include "CmApplication.h"

namespace CamelotEditor
{
	EditorWindow::EditorWindow(const String& name)
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = 1280;
		renderWindowDesc.height = 720;
		renderWindowDesc.title = "EditorWindow";
		renderWindowDesc.fullscreen = false;
		renderWindowDesc.border = WindowBorder::None;
		renderWindowDesc.toolWindow = true;

		mRenderWindow = RenderWindow::create(renderWindowDesc, gApplication().getPrimaryRenderWindow());
	}

	EditorWindow::~EditorWindow()
	{
		mRenderWindow->destroy();
	}
}