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
		return new (bs_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(getStyleName<GUITreeViewEditBox>(styleName), GUILayoutOptions::create());
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(const GUIOptions& layoutOptions, const String& styleName)
	{
		return new (bs_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(getStyleName<GUITreeViewEditBox>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	GUITreeViewEditBox::GUITreeViewEditBox(const String& styleName, const GUILayoutOptions& layoutOptions)
		:GUIInputBox(styleName, layoutOptions, false)
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