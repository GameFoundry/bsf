#include "BsGUIColorField.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIColor.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIWidget.h"
#include "BsCmdInputFieldValueChange.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	GUIColorField::GUIColorField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUIDimensions& dimensions, bool withLabel)
		:TGUIField(dummy, labelContent, labelWidth, style, dimensions, withLabel),
		mLabel(nullptr), mColor(nullptr), mLabelWidth(100)
	{
		mColor = GUIColor::create(getSubStyleName(getColorInputStyleType()));
		mColor->onValueChanged.connect(std::bind(&GUIColorField::valueChanged, this, _1));

		mLayout->addElement(mColor);
	}


	GUIColorField::~GUIColorField()
	{

	}

	void GUIColorField::setValue(const Color& color)
	{
		mValue = color;
		mColor->setColor(color);
	}

	void GUIColorField::setTint(const Color& color)
	{
		if (mLabel != nullptr)
			mLabel->setTint(color);

		mColor->setTint(color);
	}

	Vector2I GUIColorField::_getOptimalSize() const
	{
		Vector2I optimalsize = mColor->_getOptimalSize();

		if(mLabel != nullptr)
		{
			optimalsize.x += mLabel->_getOptimalSize().x;
			optimalsize.y = std::max(optimalsize.y, mLabel->_getOptimalSize().y);
		}

		return optimalsize;
	}

	void GUIColorField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mColor->setStyle(getSubStyleName(getColorInputStyleType()));
	}

	void GUIColorField::valueChanged(const Color& newValue)
	{
		setValue(newValue);

		if (!onValueChanged.empty())
			onValueChanged(newValue);
	}

	const String& GUIColorField::getGUITypeName()
	{
		static String typeName = "GUIColorField";
		return typeName;
	}

	const String& GUIColorField::getColorInputStyleType()
	{
		static String STYLE_TYPE = "EditorFieldColor";
		return STYLE_TYPE;
	}
}