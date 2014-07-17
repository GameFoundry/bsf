#include "BsGUIColorField.h"
#include "BsGUIArea.h"
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
	GUIColorField::GUIColorField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, 
		const String& style, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, style), mLabel(nullptr), mColor(nullptr), mLabelWidth(100)
	{
		mLabel = GUILabel::create(labelContent, getSubStyleName(getLabelStyleType()));
		mColor = GUIColor::create(getSubStyleName(getColorInputStyleType()));

		mColor->onValueChanged.connect(std::bind(&GUIColorField::valueChanged, this, _1));

		_registerChildElement(mLabel);
		_registerChildElement(mColor);
	}

	GUIColorField::GUIColorField(const PrivatelyConstruct& dummy, 
		const String& style, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, style), mLabel(nullptr), mColor(nullptr), mLabelWidth(100)
	{
		mColor = GUIColor::create(style);

		_registerChildElement(mColor);
	}

	GUIColorField::~GUIColorField()
	{

	}

	GUIColorField* GUIColorField::create(const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIColorField>(PrivatelyConstruct(), labelContent, *curStyle,
			GUILayoutOptions::create(layoutOptions));
	}

	GUIColorField* GUIColorField::create(const GUIContent& labelContent, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIColorField>(PrivatelyConstruct(), labelContent, *curStyle,
			GUILayoutOptions::create());
	}

	GUIColorField* GUIColorField::create(const HString& labelContent, const GUIOptions& layoutOptions, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIColorField>(PrivatelyConstruct(), GUIContent(labelContent), *curStyle,
			GUILayoutOptions::create(layoutOptions));
	}

	GUIColorField* GUIColorField::create(const HString& labelContent, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIColorField>(PrivatelyConstruct(), GUIContent(labelContent), *curStyle,
			GUILayoutOptions::create());
	}

	GUIColorField* GUIColorField::create(const GUIOptions& layoutOptions, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIColorField>(PrivatelyConstruct(), *curStyle,
			GUILayoutOptions::create(layoutOptions));
	}

	GUIColorField* GUIColorField::create(const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIColorField>(PrivatelyConstruct(), *curStyle, GUILayoutOptions::create());
	}

	void GUIColorField::setValue(const Color& color)
	{
		mValue = color;
		mColor->setColor(color);
	}

	void GUIColorField::setLabelWidth(UINT32 width)
	{
		mLabelWidth = width;

		markContentAsDirty();
	}

	void GUIColorField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 colorOffset = 0;
		UINT32 colorWidth = width;

		if(mLabel != nullptr)
		{
			UINT32 labelWidth = mLabelWidth;

			Vector2I optimalSize = mLabel->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mLabel->_setOffset(offset);
			mLabel->_setWidth(labelWidth);
			mLabel->_setHeight(optimalSize.y);
			mLabel->_setAreaDepth(areaDepth);
			mLabel->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mLabel->_setClipRect(elemClipRect);

			colorOffset = labelWidth;
			colorWidth = width - labelWidth;
		}

		{
			Vector2I optimalSize = mColor->_getOptimalSize();
			INT32 yOffset = Math::roundToInt((height - optimalSize.y) * 0.5f);

			Vector2I offset(x + colorOffset, y + yOffset);
			mColor->_setOffset(offset);
			mColor->_setWidth(colorWidth);
			mColor->_setHeight(optimalSize.y);
			mColor->_setAreaDepth(areaDepth);
			mColor->_setWidgetDepth(widgetDepth);

			RectI elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mColor->_setClipRect(elemClipRect);
		}
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

	const String& GUIColorField::getLabelStyleType()
	{
		static String STYLE_TYPE = "EditorFieldLabel";
		return STYLE_TYPE;
	}

	const String& GUIColorField::getColorInputStyleType()
	{
		static String STYLE_TYPE = "EditorFieldColor";
		return STYLE_TYPE;
	}
}