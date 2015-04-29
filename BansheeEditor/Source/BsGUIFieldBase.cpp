#include "BsGUIFieldBase.h"
#include "BsGUILabel.h"
#include "BsGUILayoutX.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"
#include "BsGUILayoutUtility.h"

namespace BansheeEngine
{
	const UINT32 GUIFieldBase::DEFAULT_LABEL_WIDTH = 100;

	GUIFieldBase::GUIFieldBase(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUIDimensions& dimensions, bool withLabel)
		:GUIElementContainer(dimensions, style), mLabel(nullptr)
	{
		mLayout = GUILayoutX::create();
		_registerChildElement(mLayout);

		if(withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), getSubStyleName(getLabelStyleType()));
			mLayout->addElement(mLabel);
		}
	}

	void GUIFieldBase::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		Rect2I clipRect, UINT8 widgetDepth, UINT16 panelDepth, UINT16 panelDepthRange)
	{
		mLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, panelDepth, panelDepthRange);
	}

	Vector2I GUIFieldBase::_getOptimalSize() const
	{
		return GUILayoutUtility::calcOptimalSize(mLayout);
	}

	void GUIFieldBase::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));
	}
}