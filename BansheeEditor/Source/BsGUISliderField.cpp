#include "BsGUISliderField.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIInputBox.h"
#include "BsGUISpace.h"
#include "BsBuiltinResources.h"
#include "BsCGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsCursor.h"
#include "BsCGUIWidget.h"
#include "BsViewport.h"
#include "BsGUISlider.h"
#include "BsCmdInputFieldValueChange.h"
#include <regex>

using namespace std::placeholders;

namespace BansheeEngine
{
	GUISliderField::GUISliderField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUIDimensions& dimensions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, style, dimensions, withLabel), mInputBox(nullptr), mSlider(nullptr)
	{
		mSlider = GUISliderHorz::create(GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(getSliderStyleType()));
		mSlider->onChanged.connect(std::bind(&GUISliderField::sliderChanged, this, _1));

		mInputBox = GUIInputBox::create(false, GUIOptions(GUIOption::fixedWidth(75)), getSubStyleName(getInputStyleType()));
		mInputBox->setFilter(&GUISliderField::floatFilter);

		mInputBox->onValueChanged.connect(std::bind((void(GUISliderField::*)(const WString&))&GUISliderField::valueChanged, this, _1));

		mLayout->addElement(mSlider);
		mLayout->addNewElement<GUIFixedSpace>(5);
		mLayout->addElement(mInputBox);

		setValueInternal(0);
		mInputBox->setText(L"0");
	}

	GUISliderField::~GUISliderField()
	{

	}

	float GUISliderField::getValue() const
	{
		return mSlider->getValue();
	}

	void GUISliderField::setValue(float value)
	{
		setValueInternal(value);

		onValueChanged(value);
	}

	void GUISliderField::setValueInternal(float value)
	{
		float origValue = mSlider->getValue();
		if (origValue != value)
			mSlider->setValue(value);

		float clampedValue = mSlider->getValue();

		// Only update with new value if it actually changed, otherwise
		// problems can occur when user types in "0." and the field
		// updates back to "0" effectively making "." unusable
		float curValue = parseFloat(mInputBox->getText());
		if (clampedValue != curValue)
			mInputBox->setText(toWString(clampedValue));
	}

	void GUISliderField::setRange(float min, float max)
	{
		mSlider->setRange(min, max);
	}

	void GUISliderField::setStep(float step)
	{
		mSlider->setStep(step);
	}

	void GUISliderField::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mInputBox->setTint(color);
	}

	const String& GUISliderField::getGUITypeName()
	{
		static String typeName = "GUISliderField";
		return typeName;
	}

	const String& GUISliderField::getInputStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFieldInput";
		return LABEL_STYLE_TYPE;
	}

	const String& GUISliderField::getSliderStyleType()
	{
		static String SLIDER_STYLE_TYPE = "EditorSliderInput";
		return SLIDER_STYLE_TYPE;
	}

	void GUISliderField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mSlider->setStyle(getSubStyleName(getSliderStyleType()));
		mInputBox->setStyle(getSubStyleName(getInputStyleType()));
	}

	void GUISliderField::valueChanged(const WString& newValue)
	{
		float newFloatValue = parseFloat(newValue);

		CmdInputFieldValueChange<GUISliderField, float>::execute(this, newFloatValue);
	}

	void GUISliderField::sliderChanged(float newValue)
	{
		setValue(mSlider->getValue());
	}

	bool GUISliderField::floatFilter(const WString& str)
	{
		return std::regex_match(str, std::wregex(L"-?(\\d+(\\.\\d*)?)?"));
	}
}