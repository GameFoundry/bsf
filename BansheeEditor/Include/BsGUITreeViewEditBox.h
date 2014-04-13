#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIInputBox.h"

namespace BansheeEditor
{
	class GUITreeViewEditBox : public BS::GUIInputBox
	{
	public:
		static const CM::String& getGUITypeName();

		static GUITreeViewEditBox* create(const CM::String& styleName = CM::StringUtil::BLANK);
		static GUITreeViewEditBox* create(const BS::GUIOptions& layoutOptions, const CM::String& styleName = CM::StringUtil::BLANK);

		boost::signal<void()> onInputConfirmed;
		boost::signal<void()> onInputCanceled;

	private:
		GUITreeViewEditBox(const CM::String& styleName, const BS::GUILayoutOptions& layoutOptions);

		virtual bool commandEvent(const BS::GUICommandEvent& ev);
	};
}