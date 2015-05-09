#include "BsGUIWidget.h"
#include "BsGUIManager.h"
#include "BsGUISkin.h"
#include "BsGUILabel.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIPanel.h"
#include "BsCoreApplication.h"
#include "BsCoreThreadAccessor.h"
#include "BsMaterial.h"
#include "BsPass.h"
#include "BsMesh.h"
#include "BsVector2I.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "BsViewport.h"
#include "BsSceneObject.h"
#include "BsRenderWindow.h"

namespace BansheeEngine
{
	GUIWidget::GUIWidget(const HSceneObject& parent, Viewport* target)
		:Component(parent), mWidgetIsDirty(false), mTarget(nullptr), mDepth(0), mPanel(nullptr)
	{
		setName("GUIWidget");

		mLastFramePosition = SO()->getWorldPosition();
		mLastFrameRotation = SO()->getWorldRotation();
		mLastFrameScale = SO()->getWorldScale();

		assert(target != nullptr);

		mTarget = target;

		mOwnerTargetResizedConn = mTarget->getTarget()->onResized.connect(std::bind(&GUIWidget::ownerTargetResized, this));

		GUIManager::instance().registerWidget(this);

		mPanel = GUIPanel::create();
		mPanel->_changeParentWidget(this);
		updatePanelSize();
	}

	GUIWidget::~GUIWidget()
	{ }

	void GUIWidget::onDestroyed()
	{
		GUILayout::destroy(mPanel);
		mPanel = nullptr;

		if (mTarget != nullptr)
		{
			GUIManager::instance().unregisterWidget(this);

			mOwnerTargetResizedConn.disconnect();
		}

		mElements.clear();
		mDirtyContents.clear();
	}

	void GUIWidget::update()
	{
		// If the widgets parent scene object moved, we need to mark it as dirty
		// as the GUIManager batching relies on object positions, so it needs to be updated.
		const float diffEpsilon = 0.0001f;

		Vector3 position = SO()->getWorldPosition();
		Quaternion rotation = SO()->getWorldRotation();
		Vector3 scale = SO()->getWorldScale();

		if(!mWidgetIsDirty)
		{
			Vector3 posDiff = mLastFramePosition - position;
			if(Math::abs(posDiff.x) > diffEpsilon || Math::abs(posDiff.y) > diffEpsilon || Math::abs(posDiff.z) > diffEpsilon)
			{
				mWidgetIsDirty = true;
			}
			else
			{
				Quaternion rotDiff = mLastFrameRotation - rotation;
				if(Math::abs(rotDiff.x) > diffEpsilon || Math::abs(rotDiff.y) > diffEpsilon || 
					Math::abs(rotDiff.z) > diffEpsilon || Math::abs(rotDiff.w) > diffEpsilon)
				{
					mWidgetIsDirty = true;
				}
				else
				{
					Vector3 scaleDiff = mLastFrameScale - scale;
					if(Math::abs(scaleDiff.x) > diffEpsilon || Math::abs(scaleDiff.y) > diffEpsilon || Math::abs(scaleDiff.z) > diffEpsilon)
					{
						mWidgetIsDirty = true;
					}
				}
			}
		}

		mLastFramePosition = position;
		mLastFrameRotation = rotation;
		mLastFrameScale = scale;
	}

	void GUIWidget::_updateLayout()
	{
		UnorderedMap<GUIElementBase*, bool> elementsToUpdate;

		// Determine dirty contents and layouts
		Stack<GUIElementBase*> todo;
		todo.push(mPanel);

		while (!todo.empty())
		{
			GUIElementBase* currentElem = todo.top();
			todo.pop();

			if (currentElem->_isDirty())
			{
				GUIElementBase* updateParent = currentElem->_getUpdateParent();
				assert(updateParent != nullptr || currentElem == mPanel);

				if (updateParent != nullptr)
				{
					if (updateParent->_getType() == GUIElementBase::Type::Panel)
					{
						GUIElementBase* optimizedUpdateParent = currentElem;
						while (optimizedUpdateParent->_getParent() != updateParent)
							optimizedUpdateParent = optimizedUpdateParent->_getParent();

						elementsToUpdate.insert(std::make_pair(optimizedUpdateParent, true));
					}
					else
						elementsToUpdate.insert(std::make_pair(updateParent, false));
				}
				else // Must be root panel
				{
					elementsToUpdate.insert(std::make_pair(mPanel, false));
				}
			}

			UINT32 numChildren = currentElem->_getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
				todo.push(currentElem->_getChild(i));
		}

		// Determine top-level layouts and update them
		for (auto& elemData : elementsToUpdate)
		{
			GUIElementBase* updateParent = nullptr;

			bool isPanelOptimized = elemData.second;
			if (isPanelOptimized)
				updateParent = elemData.first->_getParent();
			else
				updateParent = elemData.first;

			// Skip update if a parent element is also queued for update
			bool isTopLevel = true;
			GUIElementBase* curUpdateParent = elemData.first->_getParent();
			while (curUpdateParent != nullptr)
			{
				if (elementsToUpdate.find(curUpdateParent) != elementsToUpdate.end())
				{
					isTopLevel = false;
					break;
				}

				curUpdateParent = curUpdateParent->_getParent();
			}

			if (!isTopLevel)
				continue;

			_updateLayout(elemData.first);
		}
	}

	void GUIWidget::_updateLayout(GUIElementBase* elem)
	{
		GUIElementBase* parent = elem->_getParent();
		bool isPanelOptimized = parent != nullptr && parent->_getType() == GUIElementBase::Type::Panel;

		GUIElementBase* updateParent = nullptr;

		if (isPanelOptimized)
			updateParent = parent;
		else
			updateParent = elem;

		// For GUIPanel we can do a an optimization and update only the element in question instead
		// of all the children
		if (isPanelOptimized)
		{
			GUIPanel* panel = static_cast<GUIPanel*>(updateParent);

			GUIElementBase* dirtyElement = elem;
			dirtyElement->_updateOptimalLayoutSizes();

			LayoutSizeRange elementSizeRange = panel->_getElementSizeRange(dirtyElement);
			Rect2I elementArea = panel->_getElementArea(panel->_getLayoutData().area, dirtyElement, elementSizeRange);

			GUILayoutData childLayoutData = panel->_getLayoutData();
			childLayoutData.area = elementArea;

			panel->_updateChildLayout(dirtyElement, childLayoutData);
		}
		else
		{
			GUILayoutData childLayoutData = updateParent->_getLayoutData();

			updateParent->_updateLayout(childLayoutData);
		}

		// Mark dirty contents
		Stack<GUIElementBase*> todo;
		todo.push(updateParent);

		while (!todo.empty())
		{
			GUIElementBase* currentElem = todo.top();
			todo.pop();

			if (currentElem->_getType() == GUIElementBase::Type::Element)
				mDirtyContents.insert(static_cast<GUIElement*>(currentElem));

			currentElem->_markAsClean();

			UINT32 numChildren = currentElem->_getNumChildren();
			for (UINT32 i = 0; i < numChildren; i++)
				todo.push(currentElem->_getChild(i));
		}
	}

	bool GUIWidget::_mouseEvent(GUIElement* element, const GUIMouseEvent& ev)
	{
		return element->_mouseEvent(ev);
	}

	bool GUIWidget::_textInputEvent(GUIElement* element, const GUITextInputEvent& ev)
	{
		return element->_textInputEvent(ev);
	}

	bool GUIWidget::_commandEvent(GUIElement* element, const GUICommandEvent& ev)
	{
		return element->_commandEvent(ev);
	}

	bool GUIWidget::_virtualButtonEvent(GUIElement* element, const GUIVirtualButtonEvent& ev)
	{
		return element->_virtualButtonEvent(ev);
	}

	void GUIWidget::_registerElement(GUIElementBase* elem)
	{
		assert(elem != nullptr);

		if (elem->_getType() == GUIElementBase::Type::Element)
			mElements.push_back(static_cast<GUIElement*>(elem));

		mWidgetIsDirty = true;
	}

	void GUIWidget::_unregisterElement(GUIElementBase* elem)
	{
		assert(elem != nullptr);

		auto iterFind = std::find(begin(mElements), end(mElements), elem);

		if (iterFind != mElements.end())
		{
			mElements.erase(iterFind);
			mWidgetIsDirty = true;
		}

		if (elem->_getType() == GUIElementBase::Type::Element)
			mDirtyContents.erase(static_cast<GUIElement*>(elem));
	}

	void GUIWidget::_markMeshDirty(GUIElementBase* elem)
	{
		mWidgetIsDirty = true;
	}

	void GUIWidget::setSkin(const HGUISkin& skin)
	{
		mSkin = skin;

		for(auto& element : mElements)
			element->_refreshStyle();
	}

	const GUISkin& GUIWidget::getSkin() const
	{
		static const HGUISkin DEFAULT_SKIN = GUISkin::create();

		if(mSkin.isLoaded())
			return *mSkin;
		else
			return *DEFAULT_SKIN;
	}

	bool GUIWidget::isDirty(bool cleanIfDirty)
	{
		bool dirty = mWidgetIsDirty || mDirtyContents.size() > 0;

		if(cleanIfDirty && dirty)
		{
			mWidgetIsDirty = false;

			for (auto& dirtyElement : mDirtyContents)
				dirtyElement->_updateRenderElements();

			mDirtyContents.clear();
			updateBounds();
		}
		
		return dirty;
	}

	bool GUIWidget::inBounds(const Vector2I& position) const
	{
		// Technically GUI widget bounds can be larger than the viewport, so make sure we clip to viewport first
		if(!getTarget()->getArea().contains(position))
			return false;

		const Matrix4& worldTfrm = SO()->getWorldTfrm();
		Vector3 vecPos((float)position.x, (float)position.y, 0.0f);
		vecPos = worldTfrm.inverse().multiplyAffine(vecPos);

		Vector2I localPos(Math::roundToInt(vecPos.x), Math::roundToInt(vecPos.y));
		return mBounds.contains(localPos);
	}

	void GUIWidget::updateBounds() const
	{
		if(mElements.size() > 0)
			mBounds = mElements[0]->_getClippedBounds();

		for(auto& elem : mElements)
		{
			Rect2I elemBounds = elem->_getClippedBounds();
			mBounds.encapsulate(elemBounds);
		}
	}

	void GUIWidget::ownerTargetResized()
	{
		updatePanelSize();
	}

	void GUIWidget::updatePanelSize()
	{
		UINT32 width = getTarget()->getWidth();
		UINT32 height = getTarget()->getHeight();

		GUILayoutData layoutData;
		layoutData.area.width = width;
		layoutData.area.height = height;
		layoutData.clipRect = Rect2I(0, 0, width, height);

		mPanel->setWidth(width);
		mPanel->setHeight(height);

		mPanel->_setLayoutData(layoutData);
		mPanel->_markContentAsDirty();
	}

	void GUIWidget::ownerWindowFocusChanged()
	{

	}
}