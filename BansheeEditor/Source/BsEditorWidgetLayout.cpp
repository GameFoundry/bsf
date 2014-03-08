#include "BsEditorWidgetLayout.h"
#include "BsEditorWidgetLayoutRTTI.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWidgetLayout::Entry::Entry()
		:isDocked(true), x(0), y(0), width(0), height(0)
	{ }

	EditorWidgetLayout::Entry::~Entry()
	{ }

	EditorWidgetLayout::EditorWidgetLayout(const DockManagerLayoutPtr& dockLayout)
		:mDockLayout(dockLayout)
	{ }

	/************************************************************************/
	/* 								RTTI		                     		*/
	/************************************************************************/

	RTTITypeBase* EditorWidgetLayout::getRTTIStatic()
	{
		return EditorWidgetLayoutRTTI::instance();
	}

	RTTITypeBase* EditorWidgetLayout::getRTTI() const
	{
		return EditorWidgetLayout::getRTTIStatic();
	}
}