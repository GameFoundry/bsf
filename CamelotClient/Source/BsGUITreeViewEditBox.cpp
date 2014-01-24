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

	GUITreeViewEditBox* GUITreeViewEditBox::create(BS::GUIWidget& parent, const BS::GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(parent, style, GUILayoutOptions::create(style));
	}

	GUITreeViewEditBox* GUITreeViewEditBox::create(BS::GUIWidget& parent, const BS::GUIOptions& layoutOptions, const BS::GUIElementStyle* style)
	{
		if(style == nullptr)
		{
			const GUISkin& skin = parent.getSkin();
			style = skin.getStyle(getGUITypeName());
		}

		return new (cm_alloc<GUITreeViewEditBox, PoolAlloc>()) GUITreeViewEditBox(parent, style, GUILayoutOptions::create(layoutOptions, style));
	}

	GUITreeViewEditBox::GUITreeViewEditBox(BS::GUIWidget& parent, const BS::GUIElementStyle* style, const BS::GUILayoutOptions& layoutOptions)
		:GUIInputBox(parent, style, layoutOptions, false)
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

		return processed;
	}
};