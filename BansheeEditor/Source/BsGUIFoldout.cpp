#include "BsGUIFoldout.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsGUITexture.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	GUIFoldout::GUIFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style,
		const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions, style), mToggle(nullptr), mIsExpanded(false)
	{
		mLabel = GUILabel::create(label, getSubStyleName(getLabelStyleType()));
		mToggle = GUIToggle::create(HString(L""), getSubStyleName(getFoldoutButtonStyleType()));

		_registerChildElement(mLabel);
		_registerChildElement(mToggle);

		mToggle->onToggled.connect(std::bind(&GUIFoldout::toggleTriggered, this, _1));
	}

	GUIFoldout::~GUIFoldout()
	{

	}

	GUIFoldout* GUIFoldout::create(const HString& label, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIFoldout>(PrivatelyConstruct(), label, *curStyle, GUILayoutOptions::create(layoutOptions));
	}

	GUIFoldout* GUIFoldout::create(const HString& label, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIFoldout>(PrivatelyConstruct(), label, *curStyle, GUILayoutOptions::create());
	}

	void GUIFoldout::setExpanded(bool expanded)
	{
		if (mIsExpanded != expanded)
		{
			mIsExpanded = expanded;

			if (mIsExpanded)
				mToggle->toggleOn();
			else
				mToggle->toggleOff();

			markContentAsDirty();

			if (!onStateChanged.empty())
				onStateChanged(mIsExpanded);
		}
	}

	void GUIFoldout::setContent(const GUIContent& content)
	{
		mLabel->setContent(content);
	}

	void GUIFoldout::setTint(const Color& color)
	{
		mLabel->setTint(color);
		mToggle->setTint(color);
	}

	void GUIFoldout::toggleTriggered(bool value)
	{
		mIsExpanded = value;

		markContentAsDirty();

		onStateChanged(value);
	}

	void GUIFoldout::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		UINT32 toggleOffset = 0;

		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)height - optimalSize.y) * 0.5f);

			Vector2I offset(x, y + yOffset);
			mToggle->setOffset(offset);
			mToggle->setWidth(optimalSize.x);
			mToggle->setHeight(optimalSize.y);
			mToggle->_setAreaDepth(areaDepth);
			mToggle->_setWidgetDepth(widgetDepth);

			Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mToggle->_setClipRect(elemClipRect);

			toggleOffset = optimalSize.x;
		}

		{
			Vector2I optimalSize = mLabel->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)height - optimalSize.y) * 0.5f);

			Vector2I offset(x + toggleOffset, y + yOffset);
			mLabel->setOffset(offset);
			mLabel->setWidth(optimalSize.x);
			mLabel->setHeight(optimalSize.y);
			mLabel->_setAreaDepth(areaDepth);
			mLabel->_setWidgetDepth(widgetDepth);

			Rect2I elemClipRect(clipRect.x - offset.x, clipRect.y - offset.y, clipRect.width, clipRect.height);
			mLabel->_setClipRect(elemClipRect);
		}
	}

	Vector2I GUIFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();
		Vector2I labelOptimalSize = mLabel->_getOptimalSize();

		optimalsize.x += labelOptimalSize.x;
		optimalsize.y = std::max(optimalsize.y, labelOptimalSize.y);

		return optimalsize;
	}

	void GUIFoldout::styleUpdated()
	{
		mLabel->setStyle(getSubStyleName(getLabelStyleType()));
		mToggle->setStyle(getSubStyleName(getFoldoutButtonStyleType()));
	}

	const String& GUIFoldout::getGUITypeName()
	{
		static String typeName = "Foldout";
		return typeName;
	}

	const String& GUIFoldout::getFoldoutButtonStyleType()
	{
		static String FOLDOUT_BUTTON_STYLE = "FoldoutButton";
		return FOLDOUT_BUTTON_STYLE;
	}

	const String& GUIFoldout::getLabelStyleType()
	{
		static String FOLDOUT_LABEL_STYLE = "EditorFieldLabel";
		return FOLDOUT_LABEL_STYLE;
	}
}