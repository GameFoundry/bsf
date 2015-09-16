#include "BsGUIListBoxField.h"
#include "BsGUILayoutX.h"
#include "BsGUILabel.h"
#include "BsGUIListBox.h"
#include "BsBuiltinResources.h"
#include "BsCGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsCGUIWidget.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIListBoxField::DEFAULT_LABEL_WIDTH = 100;

	GUIListBoxField::GUIListBoxField(const PrivatelyConstruct& dummy, const Vector<HString>& elements, 
		const GUIContent& labelContent, UINT32 labelWidth, const String& style, const GUIDimensions& dimensions, bool withLabel)
		:GUIElementContainer(dimensions, style),
		mListBox(nullptr), mIndex(0), mLayout(nullptr), mLabel(nullptr)
	{
		mLayout = GUILayoutX::create();
		_registerChildElement(mLayout);

		if (withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), getSubStyleName(getLabelStyleType()));
			mLayout->addElement(mLabel);
		}

		mListBox = GUIListBox::create(elements, getSubStyleName(getListBoxStyleType()));
		mLayout->addElement(mListBox);

		mListBox->onSelectionChanged.connect(std::bind(&GUIListBoxField::selectionChanged, this, _1));
	}

	GUIListBoxField::~GUIListBoxField()
	{

	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const GUIContent& labelContent, UINT32 labelWidth, 
		const GUIOptions& options, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, labelContent, labelWidth, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const GUIContent& labelContent, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const HString& labelText, UINT32 labelWidth, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, GUIContent(labelText), labelWidth, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const HString& labelText, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(options), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const GUIOptions& options, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, GUIContent(), 0, *curStyle,
			GUIDimensions::create(options), false);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, labelContent, labelWidth, *curStyle,
			GUIDimensions::create(), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const GUIContent& labelContent,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const HString& labelText, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, GUIContent(labelText), labelWidth, *curStyle,
			GUIDimensions::create(), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const HString& labelText,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUIDimensions::create(), true);
	}

	GUIListBoxField* GUIListBoxField::create(const Vector<HString>& elements, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &GUIListBoxField::getGUITypeName();

		return bs_new<GUIListBoxField>(PrivatelyConstruct(), elements, GUIContent(), 0, *curStyle,
			GUIDimensions::create(), false);
	}

	void GUIListBoxField::setElements(const Vector<HString>& elements)
	{
		mListBox->setElements(elements);
	}

	void GUIListBoxField::setIndex(UINT32 index)
	{
		mIndex = index;
		mListBox->selectElement(index);
	}

	void GUIListBoxField::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mListBox->setTint(color);
	}

	void GUIListBoxField::_updateLayoutInternal(const GUILayoutData& data)
	{
		mLayout->_setLayoutData(data);
		mLayout->_updateLayoutInternal(data);
	}

	Vector2I GUIListBoxField::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
	}

	void GUIListBoxField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mListBox->setStyle(getSubStyleName(getListBoxStyleType()));
	}

	void GUIListBoxField::selectionChanged(UINT32 newIndex)
	{
		mIndex = newIndex;
		onSelectionChanged(newIndex);
	}

	const String& GUIListBoxField::getGUITypeName()
	{
		static String typeName = "GUIListBoxField";
		return typeName;
	}

	const String& GUIListBoxField::getListBoxStyleType()
	{
		static String LISTBOX_STYLE_TYPE = "EditorFieldListBox";
		return LISTBOX_STYLE_TYPE;
	}

	const String& GUIListBoxField::getLabelStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFieldLabel";
		return LABEL_STYLE_TYPE;
	}
}