#include "BsGUITextField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUITextField::GUITextField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, CM::UINT32 labelWidth, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, parent, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mInputBox(nullptr)
	{
		const GUIElementStyle* curInputBoxStyle = inputBoxStyle;

		if(curInputBoxStyle == nullptr)
			curInputBoxStyle = parent.getSkin().getStyle("InputBox");

		mInputBox = GUIInputBox::create(parent, false, inputBoxStyle);
		mLayout->addElement(mInputBox);
	}

	GUITextField::~GUITextField()
	{

	}

	WString GUITextField::getValue() const
	{
		return mInputBox->getText();
	}

	void GUITextField::setValue(const WString& value)
	{
		mInputBox->setText(value);
	}

	const String& GUITextField::getGUITypeName()
	{
		static String typeName = "GUITextField";
		return typeName;
	}
}