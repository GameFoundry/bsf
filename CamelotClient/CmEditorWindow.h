#pragma once

#include "BsPrerequisites.h"

namespace CamelotEditor
{
	class EditorWindow
	{
	public:
		EditorWindow(const CM::String& name, const CM::HFont& dbgFont, CamelotEngine::UINT32 dbgFontSize);
		virtual ~EditorWindow();

	private:
		CM::RenderWindowPtr mRenderWindow;

		BS::GUISkin* mSkin; // dbg only
	};
}