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

	GUIFieldBase::GUIFieldBase(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		GUIElementStyle* labelStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(layoutOptions)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			mLabel = BS::GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), labelStyle);
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