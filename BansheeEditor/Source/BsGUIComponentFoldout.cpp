#include "BsGUIComponentFoldout.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsGUIButton.h"
#include "BsGUITexture.h"
#include "BsBuiltinResources.h"
#include "BsCGUIWidget.h"
#include "BsGUIMouseEvent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	GUIComponentFoldout::GUIComponentFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style,
		const GUIDimensions& dimensions)
		:GUIElementContainer(dimensions, style), mToggle(nullptr), mBackground(nullptr), mIsExpanded(false)
	{
		mToggle = GUIToggle::create(label, getSubStyleName(getFoldoutButtonStyleType()));
		mBackground = GUITexture::create(getSubStyleName(getFoldoutBgStyleType()));

		_registerChildElement(mToggle);
		_registerChildElement(mBackground);

		mToggle->onToggled.connect(std::bind(&GUIComponentFoldout::toggleTriggered, this, _1));
		mBackground->_setElementDepth(2);
	}

	GUIComponentFoldout::~GUIComponentFoldout()
	{

	}

	GUIComponentFoldout* GUIComponentFoldout::create(const HString& label, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIComponentFoldout>(PrivatelyConstruct(), label, *curStyle, GUIDimensions::create(options));
	}

	GUIComponentFoldout* GUIComponentFoldout::create(const HString& label, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIComponentFoldout>(PrivatelyConstruct(), label, *curStyle, GUIDimensions::create());
	}

	void GUIComponentFoldout::setExpanded(bool expanded)
	{
		if(mIsExpanded != expanded)
		{
			mIsExpanded = expanded;

			if(mIsExpanded)
				mToggle->toggleOn();
			else
				mToggle->toggleOff();

			if(!onStateChanged.empty())
				onStateChanged(mIsExpanded);
		}
	}

	void GUIComponentFoldout::setContent(const GUIContent& content)
	{
		mToggle->setContent(content);
	}

	void GUIComponentFoldout::setTint(const Color& color)
	{
		mToggle->setTint(color);
	}

	void GUIComponentFoldout::toggleTriggered(bool value)
	{
		mIsExpanded = value;

		onStateChanged(value);
	}

	void GUIComponentFoldout::_updateLayoutInternal(const GUILayoutData& data)
	{
		Vector2I toggleOptSize = mToggle->_getOptimalSize();
		{
			INT32 yOffset = Math::roundToInt(((INT32)data.area.height - toggleOptSize.y) * 0.5f);

			GUILayoutData childData = data;
			childData.area.y += yOffset;
			childData.area.height = toggleOptSize.y;

			mToggle->_setLayoutData(childData);
		}

		mBackground->_setLayoutData(data);
	}

	Vector2I GUIComponentFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();

		return optimalsize;
	}

	void GUIComponentFoldout::styleUpdated()
	{
		mToggle->setStyle(getSubStyleName(getFoldoutButtonStyleType()));
		mBackground->setStyle(getSubStyleName(getFoldoutBgStyleType()));
	}

	const String& GUIComponentFoldout::getGUITypeName()
	{
		static String typeName = "ComponentFoldout";
		return typeName;
	}

	const String& GUIComponentFoldout::getFoldoutButtonStyleType()
	{
		static String FOLDOUT_BUTTON_STYLE = "ComponentFoldoutButton";
		return FOLDOUT_BUTTON_STYLE;		
	}

	const String& GUIComponentFoldout::getFoldoutBgStyleType()
	{
		static String FOLDOUT_BACKGROUND_STYLE = "ComponentFoldoutBg";
		return FOLDOUT_BACKGROUND_STYLE;
	}
}