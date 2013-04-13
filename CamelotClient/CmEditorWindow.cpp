#include "CmEditorWindow.h"
#include "CmRenderWindow.h"

namespace CamelotEditor
{
	EditorWindow::EditorWindow(const String& name)
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.width = 1280;
		renderWindowDesc.height = 720;
		renderWindowDesc.title = "EditorWindow";
		renderWindowDesc.fullscreen = false;

		mRenderWindow = RenderWindow::create(renderWindowDesc);
	}

	EditorWindow::~EditorWindow()
	{
		mRenderWindow->destroy();
	}
}