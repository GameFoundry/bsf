#pragma once

#include "CmPrerequisites.h"

using namespace CamelotEngine;

namespace CamelotEditor
{
	class EditorWindow
	{
	public:
		EditorWindow(const String& name, const HFont& dbgFont, CamelotEngine::UINT32 dbgFontSize);
		virtual ~EditorWindow();

	private:
		RenderWindowPtr mRenderWindow;

		GUISkin* mSkin; // dbg only
	};
}