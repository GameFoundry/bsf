#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIInputBox.h"

namespace BansheeEngine
{
	class GUITreeViewEditBox : public GUIInputBox
	{
	public:
		static const String& getGUITypeName();

		static GUITreeViewEditBox* create(const String& styleName = StringUtil::BLANK);
		static GUITreeViewEditBox* create(const GUIOptions& layoutOptions, const String& styleName = StringUtil::BLANK);

		Event<void()> onInputConfirmed;
		Event<void()> onInputCanceled;

	private:
		GUITreeViewEditBox(const String& styleName, const GUIDimensions& layoutOptions);

		virtual bool _commandEvent(const GUICommandEvent& ev);
	};
}