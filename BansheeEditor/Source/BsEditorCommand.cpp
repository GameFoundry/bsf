#include "BsEditorCommand.h"

namespace BansheeEngine
{
	void EditorCommand::destroy(EditorCommand* command)
	{
		cm_delete(command);
	}
}