#include "BsEditorCommand.h"

using namespace BansheeEngine;
using namespace CamelotFramework;

namespace BansheeEditor
{
	void EditorCommand::destroy(EditorCommand* command)
	{
		cm_delete(command);
	}
}