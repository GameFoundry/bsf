#include "BsGUITextField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"
#include "BsCmdInputFieldValueChange.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	GUITextField::GUITextField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth, 
		const String& style, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, style, layoutOptions, withLabel), 
		mInputBox(nullptr), mValue(L""), mHasInputFocus(false)
	{
		mInputBox = GUIInputBox::create(false, getSubStyleName(getInputStyleType()));
		mLayout->addElement(mInputBox);

		mInputBox->onValueChanged.connect(std::bind(&GUITextField::valueChanged, this, _1));
		mInputBox->onFocusGained.connect(std::bind(&GUITextField::focusGained, this));
		mInputBox->onFocusLost.connect(std::bind(&GUITextField::focusLost, this));
	}

	GUITextField::~GUITextField()
	{

	}

	void GUITextField::setValue(const WString& value)
	{
		mValue = value;
		mInputBox->setText(value);
	}

	void GUITextField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mInputBox->setStyle(getSubStyleName(getInputStyleType()));
	}

	void GUITextField::valueChanged(const WString& newValue)
	{
		CmdInputFieldValueChange<GUITextField, WString>::execute(this, newValue);

		if (!onValueChanged.empty())
			onValueChanged(newValue);
	}

	void GUITextField::focusGained()
	{
		UndoRedo::instance().pushGroup("InputBox");
		mHasInputFocus = true;
	}

	void GUITextField::focusLost()
	{
		UndoRedo::instance().popGroup("InputBox");
		mHasInputFocus = false;
	}

	const String& GUITextField::getGUITypeName()
	{
		static String typeName = "GUITextField";
		return typeName;
	}

	const String& GUITextField::getInputStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFieldInput";
		return LABEL_STYLE_TYPE;
	}
}