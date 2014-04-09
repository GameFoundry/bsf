#include "BsGUIToggleField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIToggleField::GUIToggleField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		UINT32 labelWidth, GUIElementStyle* labelStyle, GUIElementStyle* toggleStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, parent, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mToggle(nullptr)
	{
		const GUIElementStyle* curToggleStyle = toggleStyle;

		if(curToggleStyle == nullptr)
			curToggleStyle = parent.getSkin().getStyle("Toggle");

		mToggle = GUIToggle::create(parent, HString(L""), curToggleStyle);
		mLayout->addElement(mToggle);
	}

	bool GUIToggleField::getValue() const
	{
		return mToggle->isToggled();
	}

	void GUIToggleField::setValue(bool value)
	{
		if(value) 
			mToggle->toggleOn();
		else
			mToggle->toggleOff();
	}

	const String& GUIToggleField::getGUITypeName()
	{
		static String typeName = "GUIToggleField";
		return typeName;
	}
}