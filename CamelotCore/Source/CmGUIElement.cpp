#include "CmGUIElement.h"
#include "CmGUIWidget.h"
#include "CmGUISkin.h"
#include "CmException.h"

namespace CamelotEngine
{
	GUIElement::GUIElement(GUIWidget* parent)
		:mParent(parent)
	{
		if(parent == nullptr)
			CM_EXCEPT(InvalidParametersException, "Cannot create GUI element without providing a parent!");

		mParent->registerElement(this);
	}

	GUIElement::~GUIElement()
	{

	}

	void GUIElement::destroy(GUIElement* element)
	{
		CM_DELETE(element, GUIElement, PoolAlloc);
	}
}