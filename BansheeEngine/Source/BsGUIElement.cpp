#include "BsGUIElement.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUIElement::GUIElement(GUIWidget& parent, const GUI_LAYOUT_OPTIONS& layoutOptions)
		:mParent(parent), mIsDirty(true), mParentLayout(nullptr), mLayoutOptions(layoutOptions)
	{
		mParent.registerElement(this);
	}

	GUIElement::~GUIElement()
	{
		if(mParentLayout != nullptr)
			mParentLayout->removeElement(this);
	}

	bool GUIElement::mouseEvent(const GUIMouseEvent& ev)
	{
		return false;
	}

	void GUIElement::destroyInternal(GUIElement* element)
	{
		CM_DELETE(element, GUIElement, PoolAlloc);
	}

	void GUIElement::destroy(GUIElement* element)
	{
		element->mParent.unregisterElement(element);

		CM_DELETE(element, GUIElement, PoolAlloc);
	}
}