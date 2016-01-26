#include "BsGUIVector4Field.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIFloatField.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

namespace BansheeEngine
{
	const UINT32 GUIVector4Field::ELEMENT_LABEL_WIDTH = 10;

	GUIVector4Field::GUIVector4Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
		UINT32 labelWidth, const String& labelStyle, const String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mFieldX(nullptr), mFieldY(nullptr)
	{
		mFieldX = GUIFloatField::create(HString(L"X"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldY = GUIFloatField::create(HString(L"Y"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldZ = GUIFloatField::create(HString(L"Z"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldW = GUIFloatField::create(HString(L"W"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);

		mLayout->removeElement(mLabel);

		GUILayout* layout = &mLayout->addLayoutY();
		layout->addElement(mLabel);

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

	const String& GUIVector4Field::getGUITypeName()
	{
		static String typeName = "GUIVector4Field";
		return typeName;
	}
}