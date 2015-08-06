#include "BsEditorCommand.h"

namespace BansheeEngine
{
	EditorCommand::EditorCommand(const WString& description)
		:mDescription(description)
	{ }

	void EditorCommand::destroy(EditorCommand* command)
	{
		bs_delete(command);
	}
}