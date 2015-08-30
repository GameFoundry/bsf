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
		:GUIElementContainer(dimensions, style), mToggle(nullptr), mRemove(nullptr), mIsExpanded(false)
	{
		mToggle = GUIToggle::create(label, getSubStyleName(getFoldoutButtonStyleType()));
		mRemove = GUIButton::create(HEString(L""), getSubStyleName(getFoldoutRemoveButtonStyleType()));

		_registerChildElement(mToggle);
		_registerChildElement(mRemove);

		mToggle->onToggled.connect(std::bind(&GUIComponentFoldout::toggleTriggered, this, _1));
		mRemove->onClick.connect(std::bind(&GUIComponentFoldout::removeTriggered, this));

		mToggle->_setElementDepth(1);
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

	void GUIComponentFoldout::removeTriggered()
	{
		onRemoveClicked();
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

		{
			Vector2I optimalSize = mRemove->_getOptimalSize();

			INT32 yOffset = Math::roundToInt(((INT32)data.area.height - optimalSize.y) * 0.5f);

			GUILayoutData childData = data;
			childData.area.x = data.area.x + data.area.width - optimalSize.x - 5; // 5 = arbitrary offset
			childData.area.width = optimalSize.x;
			childData.area.y += yOffset;
			childData.area.height = optimalSize.y;

			mRemove->_setLayoutData(childData);
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
		mRemove->setStyle(getSubStyleName(getFoldoutRemoveButtonStyleType()));
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

	const String& GUIComponentFoldout::getFoldoutRemoveButtonStyleType()
	{
		static String FOLDOUT_REMOVE_BUTTON_STYLE = "ComponentFoldoutRemoveButton";
		return FOLDOUT_REMOVE_BUTTON_STYLE;
	}
}