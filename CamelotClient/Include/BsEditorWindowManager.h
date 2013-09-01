#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"
#include <boost/signals.hpp>

namespace BansheeEditor
{
	class EditorWindowManager : public CM::Module<EditorWindowManager>
	{
	public:
		~EditorWindowManager();

		EditorWindow* create();
		void destroy(EditorWindowBase* window);

		void update();
	protected:
		CM::Vector<EditorWindowBase*>::type mEditorWindows;
		CM::Vector<EditorWindowBase*>::type mScheduledForDestruction;
	};
}