#include "BsEditorWidget.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUITexture.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsEngineGUI.h"
#include "BsGUIArea.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWidget::EditorWidget(const WString& name)
		:mName(name), mParentWidget(nullptr)
	{
		
	}

	EditorWidget::~EditorWidget()
	{

	}

	void EditorWidget::_initialize(GUIWidget& widget, INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		_changeParent(widget);
		_setArea(x, y, width, height);
	}

	void EditorWidget::_setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		if(mContent != nullptr)
			return;

		mContent->setPosition(x, y);
		mContent->setSize(width, height);
	}

	void EditorWidget::_changeParent(BS::GUIWidget& widget)
	{
		if(mParentWidget != &widget) 
		{
			// TODO - GUIArea mContent should be recreated (or moved) to the new widget
		}

		mParentWidget = &widget;
		mContent = GUIArea::create(widget, 0, 0, 0, 0, 10000);
	}
}