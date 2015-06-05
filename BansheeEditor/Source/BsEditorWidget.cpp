#include "BsEditorWidget.h"
#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUITexture.h"
#include "BsGUISkin.h"
#include "BsGUILayout.h"
#include "BsGUIPanel.h"
#include "BsBuiltinResources.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWidgetManager.h"
#include "BsEditorWindow.h"
#include "BsRenderWindow.h"

namespace BansheeEngine
{
	EditorWidgetBase::EditorWidgetBase(const HString& displayName, const String& name, EditorWidgetContainer& parentContainer)
		:mDisplayName(displayName), mName(name), mParent(nullptr), mContent(nullptr), mX(0), mY(0), mWidth(0), mHeight(0), mHasFocus(false)
	{
		parentContainer.add(*this);
	}

	EditorWidgetBase::~EditorWidgetBase()
	{
		if (mContent != nullptr)
			GUILayout::destroy(mContent);
	}

	EditorWindowBase* EditorWidgetBase::getParentWindow() const
	{
		return mParent->getParentWindow();
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
		if (mX == x && mY == y)
			return;

		mX = x;
		mY = y;

		if(mContent != nullptr)
			mContent->setPosition(x, y);

		doOnMoved(x, y);
	}

	void EditorWidgetBase::_setSize(UINT32 width, UINT32 height)
	{
		if (mWidth == width && mHeight == height)
			return;

		mWidth = width;
		mHeight = height;

		if (mContent != nullptr)
		{
			mContent->setWidth(width);
			mContent->setHeight(height);
		}

		doOnResized(width, height);
	}

	void EditorWidgetBase::_setHasFocus(bool focus) 
	{ 
		if (mHasFocus != focus)
		{
			mHasFocus = focus;

			onFocusChanged(focus);
		}
	}

	Vector2I EditorWidgetBase::screenToWidgetPos(const Vector2I& screenPos) const
	{
		if (mParent == nullptr)
			return screenPos;

		EditorWindowBase* parentEditorWindow = mParent->getParentWindow();
		RenderWindowPtr parentRenderWindow = parentEditorWindow->getRenderWindow();

		Vector2I windowPos = parentRenderWindow->screenToWindowPos(screenPos);
		windowPos.x -= mX;
		windowPos.y -= mY;

		return windowPos;
	}

	Vector2I EditorWidgetBase::widgetToScreenPos(const Vector2I& widgetPos) const
	{
		if (mParent == nullptr)
			return widgetPos;

		EditorWindowBase* parentEditorWindow = mParent->getParentWindow();
		RenderWindowPtr parentRenderWindow = parentEditorWindow->getRenderWindow();

		Vector2I windowPos = widgetPos;
		windowPos.x += mX;
		windowPos.y += mY;

		return parentRenderWindow->windowToScreenPos(windowPos);
	}

	void EditorWidgetBase::doOnMoved(INT32 x, INT32 y)
	{
		onMoved(x, y);
	}

	void EditorWidgetBase::doOnResized(UINT32 width, UINT32 height)
	{
		onResized(width, height);
	}

	void EditorWidgetBase::doOnParentChanged()
	{

	}

	// Note: Must not be virtual as parent container uses it in constructor
	void EditorWidgetBase::_changeParent(EditorWidgetContainer* parent)
	{
		if(mParent != parent) 
		{
			if(parent != nullptr)
			{
				if (mContent == nullptr)
					mContent = parent->getParentWidget().getPanel()->addNewElement<GUIPanel>();
				else
					parent->getParentWidget().getPanel()->addElement(mContent);
			}
			else
			{
				if (mContent != nullptr)
					mParent->getParentWidget().getPanel()->removeElement(mContent);
			}

			mParent = parent;

			doOnParentChanged();

			if(!onParentChanged.empty())
				onParentChanged(mParent);
		}
	}

	void EditorWidgetBase::_disable()
	{
		mContent->disableRecursively();
	}

	void EditorWidgetBase::_enable()
	{
		mContent->enableRecursively();
	}

	GUIWidget& EditorWidgetBase::getParentWidget() const
	{
		return mParent->getParentWidget();
	}
}