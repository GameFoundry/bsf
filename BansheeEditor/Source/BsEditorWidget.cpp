#include "BsEditorWidget.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUITexture.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsBuiltinResources.h"
#include "BsGUIArea.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWidgetManager.h"

namespace BansheeEngine
{
	EditorWidgetBase::EditorWidgetBase(const HString& displayName, const String& name, EditorWidgetContainer& parentContainer)
		:mDisplayName(displayName), mName(name), mParent(nullptr), mContent(nullptr), mX(0), mY(0), mWidth(0), mHeight(0)
	{
		parentContainer.add(*this);
	}

	EditorWidgetBase::~EditorWidgetBase()
	{
		if (mContent != nullptr)
			GUIArea::destroy(mContent);
	}

	void EditorWidgetBase::close()
	{
		EditorWidgetManager::instance().close(this);
	}

	void EditorWidgetBase::destroy(EditorWidgetBase* widget)
	{
		widget->~EditorWidgetBase();
		bs_free(widget);
	}

	void EditorWidgetBase::_setPosition(INT32 x, INT32 y)
	{
		mX = x;
		mY = y;

		if(mContent != nullptr)
			mContent->setPosition(x, y);

		if(!onMoved.empty())
			onMoved(x, y);
	}

	void EditorWidgetBase::_setSize(UINT32 width, UINT32 height)
	{
		mWidth = width;
		mHeight = height;

		if(mContent != nullptr)
			mContent->setSize(width, height);

		if(!onResized.empty())
			onResized(width, height);
	}

	// Note: Must not be virtual as parent container uses it in constructor
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

			if(!onParentChanged.empty())
				onParentChanged(mParent);
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