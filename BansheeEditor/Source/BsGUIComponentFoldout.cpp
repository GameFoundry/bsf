#include "BsGUIComponentFoldout.h"
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
	GUIComponentFoldout::GUIComponentFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style,
		const GUIDimensions& dimensions)
		:GUIElementContainer(dimensions, style), mToggle(nullptr), mIsExpanded(false)
	{
		mToggle = GUIToggle::create(label, getSubStyleName(getFoldoutButtonStyleType()));

		_registerChildElement(mToggle);

		mToggle->onToggled.connect(std::bind(&GUIComponentFoldout::toggleTriggered, this, _1));
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
		UINT32 toggleOffset = 0;
		
		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)data.area.height - optimalSize.y) * 0.5f);

			GUILayoutData childData = data;
			childData.area.y += yOffset;
			childData.area.height = optimalSize.y;

			mToggle->_setLayoutData(childData);

			toggleOffset = optimalSize.x;
		}
	}

	Vector2I GUIComponentFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();

		return optimalsize;
	}

	void GUIComponentFoldout::styleUpdated()
	{
		mToggle->setStyle(getSubStyleName(getFoldoutButtonStyleType()));
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
}