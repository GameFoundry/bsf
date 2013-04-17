#include "BsGUIElement.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "CmException.h"

using namespace CamelotFramework;

namespace BansheeEngine
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