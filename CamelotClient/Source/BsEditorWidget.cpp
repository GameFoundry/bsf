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
	EditorWidgetBase::EditorWidgetBase(const HString& name)
		:mName(name), mParent(nullptr), mContent(nullptr)
	{
		
	}

	EditorWidgetBase::~EditorWidgetBase()
	{

	}

	void EditorWidgetBase::destroy(EditorWidgetBase* widget)
	{
		cm_delete(widget);
	}

	void EditorWidgetBase::_setPosition(INT32 x, INT32 y)
	{
		if(mContent == nullptr)
			return;

		mContent->setPosition(x, y);
	}

	void EditorWidgetBase::_setSize(UINT32 width, UINT32 height)
	{
		if(mContent == nullptr)
			return;

		mContent->setSize(width, height);
	}

	void EditorWidgetBase::_changeParent(EditorWidgetContainer* parent)
	{
		if(mParent != parent) 
		{
			if(parent != nullptr)
			{
				if(mContent == nullptr)
					mContent = GUIArea::create(parent->getParentWidget(), 0, 0, 0, 0, 10000);
				else
					mContent->changeParentWidget(&parent->getParentWidget());
			}
			else
			{
				if(mContent != nullptr)
					mContent->changeParentWidget(nullptr);
			}

			mParent = parent;
		}
	}

	void EditorWidgetBase::_disable()
	{
		mContent->disable();
	}

	void EditorWidgetBase::_enable()
	{
		mContent->enable();
	}

	GUIWidget& EditorWidgetBase::getParentWidget() const
	{
		return mParent->getParentWidget();
	}
}