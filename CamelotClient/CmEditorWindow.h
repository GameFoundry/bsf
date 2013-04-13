#pragma once

#include "CmPrerequisites.h"

using namespace CamelotEngine;

namespace CamelotEditor
{
	class EditorWindow
	{
	public:
		EditorWindow(const String& name);
		virtual ~EditorWindow();

	private:
		RenderWindowPtr mRenderWindow;
	};
}