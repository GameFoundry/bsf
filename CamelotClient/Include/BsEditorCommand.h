#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	class EditorCommand
	{
	public:
		virtual ~EditorCommand() { }

		virtual void commit() { }
		virtual void revert() { }

		static void destroy(EditorCommand* command);
	};
}