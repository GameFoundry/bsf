#pragma once

#include "BsPrerequisites.h"

namespace BansheeEditor
{
	class EditorWindow
	{
	public:
		EditorWindow(const CM::String& name);
		virtual ~EditorWindow();

	protected:
		virtual void update();

	private:
		CM::RenderWindowPtr mRenderWindow;

		BS::GUILabel* mDbgLabel;
	};
}