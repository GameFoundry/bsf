#include "BsEditorWidget.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUITexture.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsEngineGUI.h"
#include "BsGUIArea.h"
#include "BsEditorWidgetContainer.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	EditorWidget::EditorWidget(const WString& name)
		:mName(name), mParent(nullptr), mContent(nullptr)
	{
		
	}

	EditorWidget::~EditorWidget()
	{

	}

	void EditorWidget::destroy(EditorWidget* widget)
	{
		cm_delete(widget);
	}

	void EditorWidget::_setPosition(INT32 x, INT32 y)
	{
		if(mContent == nullptr)
			return;

		mContent->setPosition(x, y);
	}

	void EditorWidget::_setSize(UINT32 width, UINT32 height)
	{
		if(mContent == nullptr)
			return;

		mContent->setSize(width, height);
	}

	void EditorWidget::_changeParent(EditorWidgetContainer* parent)
	{
		if(mParent != parent) 
		{
			if(mParent == nullptr)
				mContent = GUIArea::create(parent->getParentWidget(), 0, 0, 0, 0, 10000);
			else
				mContent->changeParentWidget(parent->getParentWidget());

			mParent = parent;
		}
	}

	void EditorWidget::_disable()
	{
		mContent->disable();
	}

	void EditorWidget::_enable()
	{
		mContent->enable();
	}

	GUIWidget& EditorWidget::getParentWidget() const
	{
		return mParent->getParentWidget();
	}
}