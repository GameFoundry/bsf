#include "BsEditorWindow.h"
#include "BsEditorWidgetContainer.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWindow::EditorWindow()
		:mWidgets(cm_new<EditorWidgetContainer>())
	{

	}

	EditorWindow::~EditorWindow()
	{
		cm_delete(mWidgets);
	}
}