#include "BsEditorCommand.h"

namespace BansheeEngine
{
	void EditorCommand::destroy(EditorCommand* command)
	{
		bs_delete(command);
	}
}