#include "BsGUITreeViewEditBox.h"
#include "BsGUICommandEvent.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"

namespace BansheeEngine
{
	const String& GUITreeViewEditBox::getGUITypeName()
	{
		static String name = "TreeViewEditBox";
		return name;
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(const String& styleName)
	{
		return new (bs_alloc<GUITreeViewEditBox>()) GUITreeViewEditBox(getStyleName<GUITreeViewEditBox>(styleName), GUIDimensions::create());
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(const GUIOptions& options, const String& styleName)
	{
		return new (bs_alloc<GUITreeViewEditBox>()) GUITreeViewEditBox(getStyleName<GUITreeViewEditBox>(styleName), GUIDimensions::create(options));
	}

	GUITreeViewEditBox::GUITreeViewEditBox(const String& styleName, const GUIDimensions& dimensions)
		:GUIInputBox(styleName, dimensions, false)
	{

	}

	bool GUITreeViewEditBox::_commandEvent(const GUICommandEvent& ev)
	{
		bool processed = GUIInputBox::_commandEvent(ev);

		if(ev.getType() == GUICommandEventType::Return)
		{
			if(!onInputConfirmed.empty())
				onInputConfirmed();

			return true;
		}
		else if(ev.getType() == GUICommandEventType::Escape)
		{
			if(!onInputCanceled.empty())
				onInputCanceled();

			return true;
		}
		else if(ev.getType() == GUICommandEventType::FocusLost)
		{
			if(!onInputCanceled.empty())
				onInputCanceled();

			return true;
		}

		return processed;
	}
};