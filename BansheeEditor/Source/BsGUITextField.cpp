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
	GUITextField::GUITextField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, CM::UINT32 labelWidth, 
		GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mInputBox(nullptr)
	{
		mInputBox = GUIInputBox::create(false, inputBoxStyle);
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