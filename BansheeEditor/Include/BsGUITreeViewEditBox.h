#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIInputBox.h"

namespace BansheeEditor
{
	class GUITreeViewEditBox : public BS::GUIInputBox
	{
	public:
		static const CM::String& getGUITypeName();

		static GUITreeViewEditBox* create(const BS::GUIElementStyle* style = nullptr);
		static GUITreeViewEditBox* create(const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style = nullptr);

		boost::signal<void()> onInputConfirmed;
		boost::signal<void()> onInputCanceled;

	private:
		GUITreeViewEditBox(const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions);

		virtual bool commandEvent(const BS::GUICommandEvent& ev);
	};
}