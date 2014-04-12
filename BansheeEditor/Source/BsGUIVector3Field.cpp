#include "BsGUIVector3Field.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIFloatField.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUIVector3Field::ELEMENT_LABEL_WIDTH = 10;

	GUIVector3Field::GUIVector3Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
		CM::UINT32 labelWidth, GUIElementStyle* labelStyle, GUIElementStyle* inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mFieldX(nullptr), mFieldY(nullptr)
	{
		mFieldX = GUIFloatField::create(HString(L"X"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldY = GUIFloatField::create(HString(L"Y"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldZ = GUIFloatField::create(HString(L"Z"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);

		mLayout->removeElement(mLabel);

		GUILayout* layout = &mLayout->addLayoutY();
		layout->addElement(mLabel);

		GUILayout* elementLayout = &layout->addLayoutX();

		elementLayout->addElement(mFieldX);
		elementLayout->addElement(mFieldY);
		elementLayout->addElement(mFieldZ);
	}

	Vector3 GUIVector3Field::getValue() const
	{
		Vector3 value;
		value.x = mFieldX->getValue();
		value.y = mFieldY->getValue();
		value.z = mFieldZ->getValue();

		return value;
	}

	void GUIVector3Field::setValue(const Vector3& value)
	{
		mFieldX->setValue(value.x);
		mFieldY->setValue(value.y);
		mFieldZ->setValue(value.z);
	}

	const String& GUIVector3Field::getGUITypeName()
	{
		static String typeName = "GUIVector3Field";
		return typeName;
	}
}