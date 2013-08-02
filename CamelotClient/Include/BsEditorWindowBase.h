#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEditor
{
	class EditorWindowBase
	{
	public:
		EditorWindowBase();
		virtual ~EditorWindowBase();

		void setPosition(CM::INT32 x, CM::INT32 y);
		void setSize(CM::UINT32 width, CM::UINT32 height);

	private:
		CM::RenderWindowPtr mRenderWindow;

		CM::HSceneObject mSceneObject;
		BS::HGUIWidget mGUI;
	};
}