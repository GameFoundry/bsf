#include "BsGUITextField.h"
#include "BsGUIArea.h"
#include "BsGUILayoutX.h"
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
	const UINT32 GUITextField::DEFAULT_LABEL_WIDTH = 100;

	GUITextField::GUITextField(const PrivatelyConstruct& dummy, bool multiline, const GUIContent& labelContent, 
		UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel)
		:GUIElementContainer(dimensions, style),
		mInputBox(nullptr), mValue(L""), mHasInputFocus(false), mLayout(nullptr), mLabel(nullptr)
	{
		mLayout = GUILayoutX::create();
		_registerChildElement(mLayout);

		if (withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), getSubStyleName(getLabelStyleType()));
			mLayout->addElement(mLabel);
		}

		mInputBox = GUIInputBox::create(multiline, getSubStyleName(getInputStyleType()));
		mLayout->addElement(mInputBox);

		mInputBox->onValueChanged.connect(std::bind(&GUITextField::valueChanged, this, _1));
		mInputBox->onFocusGained.connect(std::bind(&GUITextField::focusGained, this));
		mInputBox->onFocusLost.connect(std::bind(&GUITextField::focusLost, this));
	}

	GUITextField::~GUITextField()
	{

	}

	GUITextField* GUITextField::create(bool multiline, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, labelContent, labelWidth, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUITextField* GUITextField::create(bool multiline, const GUIContent& labelContent, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUITextField* GUITextField::create(bool multiline, const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, GUIContent(labelText), labelWidth, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUITextField* GUITextField::create(bool multiline, const HString& labelText, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUITextField* GUITextField::create(bool multiline, const GUIOptions& options, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, GUIContent(), 0, *curStyle,
			GUIDimensions::create(options), false);
	}

	GUITextField* GUITextField::create(bool multiline, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, labelContent, labelWidth, *curStyle,
			GUIDimensions::create(), true);
	}

	GUITextField* GUITextField::create(bool multiline, const GUIContent& labelContent,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(), true);
	}

	GUITextField* GUITextField::create(bool multiline, const HString& labelText, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, GUIContent(labelText), labelWidth, *curStyle,
			GUIDimensions::create(), true);
	}

	GUITextField* GUITextField::create(bool multiline, const HString& labelText,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(), true);
	}

	GUITextField* GUITextField::create(bool multiline, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUITextField::getGUITypeName();

		return bs_new<GUITextField>(PrivatelyConstruct(), multiline, GUIContent(), 0, *curStyle,
			GUIDimensions::create(), false);
	}

	void GUITextField::setValue(const WString& value)
	{
		mValue = value;
		mInputBox->setText(value);
	}

	void GUITextField::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mInputBox->setTint(color);
	}

	void GUITextField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 panelDepth, UINT16 panelDepthRange)
	{
		mLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, panelDepth, panelDepthRange);
	}

	Vector2I GUITextField::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
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

	const String& GUITextField::getLabelStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFieldLabel";
		return LABEL_STYLE_TYPE;
	}
}