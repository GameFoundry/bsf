#include "BsGUIVector2Field.h"
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
	const UINT32 GUIVector2Field::ELEMENT_LABEL_WIDTH = 10;

	GUIVector2Field::GUIVector2Field(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
		CM::UINT32 labelWidth, const CM::String& labelStyle, const CM::String& inputBoxStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, labelStyle, layoutOptions, withLabel), mFieldX(nullptr), mFieldY(nullptr)
	{
		mFieldX = GUIFloatField::create(HString(L"X"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);
		mFieldY = GUIFloatField::create(HString(L"Y"), ELEMENT_LABEL_WIDTH, labelStyle, inputBoxStyle);

		mLayout->removeElement(mLabel);

		GUILayout* layout = &mLayout->addLayoutY();
		layout->addElement(mLabel);

		GUILayout* elementLayout = &layout->addLayoutX();

		elementLayout->addElement(mFieldX);
		elementLayout->addElement(mFieldY);
	}

	Vector2 GUIVector2Field::getValue() const
	{
		Vector2 value;
		value.x = mFieldX->getValue();
		value.y = mFieldY->getValue();

		return value;
	}

	void GUIVector2Field::setValue(const Vector2& value)
	{
		mFieldX->setValue(value.x);
		mFieldY->setValue(value.y);
	}

	const String& GUIVector2Field::getGUITypeName()
	{
		static String typeName = "GUIVector2Field";
		return typeName;
	}
}