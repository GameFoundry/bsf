#include "BsGUITreeViewEditBox.h"
#include "BsGUICommandEvent.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const CM::String& GUITreeViewEditBox::getGUITypeName()
	{
		static String name = "TreeViewEditBox";
		return name;
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(const CM::String& styleName)
	{
		return new (cm_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(getStyleName<GUITreeViewEditBox>(styleName), GUILayoutOptions::create());
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(const BS::GUIOptions& layoutOptions, const CM::String& styleName)
	{
		return new (cm_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(getStyleName<GUITreeViewEditBox>(styleName), GUILayoutOptions::create(layoutOptions));
	}

	GUITreeViewEditBox::GUITreeViewEditBox(const CM::String& styleName, const BS::GUILayoutOptions& layoutOptions)
		:GUIInputBox(styleName, layoutOptions, false)
	{

	}

	bool GUITreeViewEditBox::commandEvent(const BS::GUICommandEvent& ev)
	{
		bool processed = GUIInputBox::commandEvent(ev);

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