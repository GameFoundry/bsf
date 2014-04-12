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

	GUITreeViewEditBox* GUITreeViewEditBox::create(const BS::GUIElementStyle* style)
	{
		return new (cm_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(style, GUILayoutOptions::create(style));
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style)
	{
		return new (cm_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(style, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITreeViewEditBox::GUITreeViewEditBox(const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions)
		:GUIInputBox(style, layoutOptions, false)
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