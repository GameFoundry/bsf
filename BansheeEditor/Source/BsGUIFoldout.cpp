#include "BsGUIFoldout.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIToggle.h"
#include "BsGUITexture.h"
#include "BsBuiltinResources.h"
#include "BsCGUIWidget.h"
#include "BsGUIMouseEvent.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	GUIFoldout::GUIFoldout(const PrivatelyConstruct& dummy, const HString& label, const String& style,
		const GUIDimensions& dimensions)
		:GUIElementContainer(dimensions, style), mToggle(nullptr), mIsExpanded(false)
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

	GUIFoldout* GUIFoldout::create(const HString& label, const GUIOptions& options,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIFoldout>(PrivatelyConstruct(), label, *curStyle, GUIDimensions::create(options));
	}

	GUIFoldout* GUIFoldout::create(const HString& label, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIFoldout>(PrivatelyConstruct(), label, *curStyle, GUIDimensions::create());
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

		onStateChanged(value);
	}

	void GUIFoldout::_updateLayoutInternal(const GUILayoutData& data)
	{
		UINT32 toggleOffset = 0;

		{
			Vector2I optimalSize = mToggle->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)data.area.height - optimalSize.y) * 0.5f);

			GUILayoutData childData = data;
			childData.area.y += yOffset;
			childData.area.width = optimalSize.x;
			childData.area.height = optimalSize.y;

			mToggle->_setLayoutData(childData);

			toggleOffset = optimalSize.x + 3; // +3 = arbitrary spacing
		}

		{
			Vector2I optimalSize = mLabel->_getOptimalSize();
			INT32 yOffset = Math::roundToInt(((INT32)data.area.height - optimalSize.y) * 0.5f);

			GUILayoutData childData = data;
			childData.area.x += toggleOffset;
			childData.area.y += yOffset;
			childData.area.width = optimalSize.x;
			childData.area.height = optimalSize.y;

			mLabel->_setLayoutData(childData);
		}
	}

	Vector2I GUIFoldout::_getOptimalSize() const
	{
		Vector2I optimalsize = mToggle->_getOptimalSize();
		Vector2I labelOptimalSize = mLabel->_getOptimalSize();

		optimalsize.x += labelOptimalSize.x;
		optimalsize.y = std::max(optimalsize.y, labelOptimalSize.y);

		return mDimensions.calculateSizeRange(optimalsize).optimal;
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