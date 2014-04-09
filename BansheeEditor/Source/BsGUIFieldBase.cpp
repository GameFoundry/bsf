#include "BsGUIFieldBase.h"
#include "BsGUILabel.h"
#include "BsGUILayout.h"
#include "BsGUIWidget.h"
#include "BsGUISkin.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	const UINT32 GUIFieldBase::DEFAULT_LABEL_WIDTH = 100;

	GUIFieldBase::GUIFieldBase(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, UINT32 labelWidth,
		GUIElementStyle* labelStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(parent, layoutOptions)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			const GUIElementStyle* curLabelStyle = labelStyle;
			if(curLabelStyle == nullptr)
				curLabelStyle = parent.getSkin().getStyle("Label");

			mLabel = BS::GUILabel::create(parent, labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), curLabelStyle);
			mLayout->addElement(mLabel);
		}
	}

	void GUIFieldBase::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		mLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
	}

	Vector2I GUIFieldBase::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
	}
}