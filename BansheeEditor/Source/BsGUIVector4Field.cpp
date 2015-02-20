#include "BsGUIVector4Field.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIFloatField.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUISpace.h"
#include "BsGUIWidget.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIVector4Field::ELEMENT_LABEL_WIDTH = 10;

	GUIVector4Field::GUIVector4Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
		UINT32 labelWidth, const String& style, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, style, layoutOptions, withLabel), mFieldX(nullptr), mFieldY(nullptr)
	{
		mFieldX = GUIFloatField::create(HString(L"X"), ELEMENT_LABEL_WIDTH, getSubStyleName(getFloatFieldStyleType()));
		mFieldY = GUIFloatField::create(HString(L"Y"), ELEMENT_LABEL_WIDTH, getSubStyleName(getFloatFieldStyleType()));
		mFieldZ = GUIFloatField::create(HString(L"Z"), ELEMENT_LABEL_WIDTH, getSubStyleName(getFloatFieldStyleType()));
		mFieldW = GUIFloatField::create(HString(L"W"), ELEMENT_LABEL_WIDTH, getSubStyleName(getFloatFieldStyleType()));

		mFieldX->onValueChanged.connect(std::bind(&GUIVector4Field::valueChanged, this, _1));
		mFieldY->onValueChanged.connect(std::bind(&GUIVector4Field::valueChanged, this, _1));
		mFieldZ->onValueChanged.connect(std::bind(&GUIVector4Field::valueChanged, this, _1));
		mFieldW->onValueChanged.connect(std::bind(&GUIVector4Field::valueChanged, this, _1));

		mLayout->removeElement(mLabel);

		GUILayout* layout = &mLayout->addLayoutY();
		layout->addElement(mLabel);
		mLabel->setLayoutOptions(GUIOptions());

		GUILayout* elementLayout = &layout->addLayoutX();

		elementLayout->addElement(mFieldX);
		elementLayout->addElement(mFieldY);
		elementLayout->addElement(mFieldZ);
		elementLayout->addElement(mFieldW);
	}

	Vector4 GUIVector4Field::getValue() const
	{
		Vector4 value;
		value.x = mFieldX->getValue();
		value.y = mFieldY->getValue();
		value.z = mFieldZ->getValue();
		value.w = mFieldW->getValue();

		return value;
	}

	void GUIVector4Field::setValue(const Vector4& value)
	{
		mFieldX->setValue(value.x);
		mFieldY->setValue(value.y);
		mFieldZ->setValue(value.z);
		mFieldW->setValue(value.w);
	}

	bool GUIVector4Field::hasInputFocus() const
	{
		return mFieldX->hasInputFocus() || mFieldY->hasInputFocus() || mFieldZ->hasInputFocus() || mFieldW->hasInputFocus();
	}

	void GUIVector4Field::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mFieldX->setTint(color);
		mFieldY->setTint(color);
		mFieldZ->setTint(color);
		mFieldW->setTint(color);
	}

	void GUIVector4Field::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mFieldX->setStyle(getSubStyleName(getFloatFieldStyleType()));
		mFieldY->setStyle(getSubStyleName(getFloatFieldStyleType()));
		mFieldZ->setStyle(getSubStyleName(getFloatFieldStyleType()));
		mFieldW->setStyle(getSubStyleName(getFloatFieldStyleType()));
	}

	void GUIVector4Field::valueChanged(float newValue)
	{
		onValueChanged(getValue());
	}

	const String& GUIVector4Field::getGUITypeName()
	{
		static String typeName = "GUIVector4Field";
		return typeName;
	}

	const String& GUIVector4Field::getFloatFieldStyleType()
	{
		static String LABEL_STYLE_TYPE = "EditorFloatField";
		return LABEL_STYLE_TYPE;
	}
}