#include "BsGUIToggleField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

namespace BansheeEngine
{
	GUIToggleField::GUIToggleField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
		UINT32 labelWidth, const String& labelStyle, const String& toggleStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mToggle(nullptr)
	{
		mToggle = GUIToggle::create(HString(L""), toggleStyle);
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