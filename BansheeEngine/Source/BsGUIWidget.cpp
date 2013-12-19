#include "BsGUIWidget.h"
#include "BsGUIManager.h"
#include "BsGUISkin.h"
#include "BsGUILabel.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIArea.h"
#include "CmApplication.h"
#include "CmCoreThreadAccessor.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmMesh.h"
#include "CmVector2I.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "CmViewport.h"
#include "CmSceneObject.h"
#include "CmRenderWindow.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUISkin GUIWidget::DefaultSkin;

	GUIWidget::GUIWidget(const HSceneObject& parent, CM::Viewport* target)
		:Component(parent), mSkin(nullptr), mWidgetIsDirty(false), mTarget(nullptr), mDepth(0)
	{
		mLastFramePosition = SO()->getWorldPosition();
		mLastFrameRotation = SO()->getWorldRotation();
		mLastFrameScale = SO()->getWorldScale();

		assert(target != nullptr);

		mTarget = target;

		mOwnerTargetResizedConn = mTarget->onResized.connect(boost::bind(&GUIWidget::ownerTargetResized, this));

		GUIManager::instance().registerWidget(this);
	}

	GUIWidget::~GUIWidget()
	{
		if(mTarget != nullptr)
		{
			GUIManager::instance().unregisterWidget(this);

			mOwnerTargetResizedConn.disconnect();
		}

		// Iterate over all elements in this way because each
		// GUIElement::destroy call internally unregisters the element
		// from the widget, and modifies the mElements array
		while(mElements.size() > 0)
		{
			GUIElement::destroy(mElements[0]);
		}

		for(auto& area : mAreas)
		{
			GUIArea::destroyInternal(area);
		}

		mElements.clear();
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
		for(auto& area : mAreas)
		{
			area->_update();
		}
	}

	bool GUIWidget::_mouseEvent(GUIElement* element, const GUIMouseEvent& ev)
	{
		// If an element has any parents we send the events to all parents first and only then to the children unless
		// the parents process them
		Stack<GUIElement*>::type todo;
		GUIElementBase* curElement = element;
		do
		{
			if(curElement->_getType() == GUIElementBase::Type::Element)
				todo.push(static_cast<GUIElement*>(curElement));

			curElement = curElement->_getParent();
		} while(curElement != nullptr);
		
		while(true)
		{
			GUIElement* elem = todo.top();
			todo.pop();

			if(elem->mouseEvent(ev))
				return true;

			if(todo.size() == 0)
				return false;
		}

		return false;
	}

	bool GUIWidget::_textInputEvent(GUIElement* element, const GUITextInputEvent& ev)
	{
		// If an element has any parents we send the events to all parents first and only then to the children unless
		// the parents process them
		Stack<GUIElement*>::type todo;
		GUIElementBase* curElement = element;
		do
		{
			if(curElement->_getType() == GUIElementBase::Type::Element)
				todo.push(static_cast<GUIElement*>(curElement));

			curElement = curElement->_getParent();
		} while(curElement != nullptr);

		while(true)
		{
			GUIElement* elem = todo.top();
			todo.pop();

			if(elem->textInputEvent(ev))
				return true;

			if(todo.size() == 0)
				return false;
		}

		return false;
	}

	bool GUIWidget::_commandEvent(GUIElement* element, const GUICommandEvent& ev)
	{
		// If an element has any parents we send the events to all parents first and only then to the children unless
		// the parents process them
		Stack<GUIElement*>::type todo;
		GUIElementBase* curElement = element;
		do
		{
			if(curElement->_getType() == GUIElementBase::Type::Element)
				todo.push(static_cast<GUIElement*>(curElement));

			curElement = curElement->_getParent();
		} while(curElement != nullptr);

		while(true)
		{
			GUIElement* elem = todo.top();
			todo.pop();

			if(elem->commandEvent(ev))
				return true;

			if(todo.size() == 0)
				return false;
		}

		return false;
	}

	void GUIWidget::registerElement(GUIElement* elem)
	{
		assert(elem != nullptr);

		mElements.push_back(elem);

		mWidgetIsDirty = true;
	}

	void GUIWidget::unregisterElement(GUIElement* elem)
	{
		assert(elem != nullptr);

		auto iterFind = std::find(begin(mElements), end(mElements), elem);

		if(iterFind == mElements.end())
			CM_EXCEPT(InvalidParametersException, "Cannot unregister an element that is not registered on this widget.");

		mElements.erase(iterFind);
		mWidgetIsDirty = true;
	}

	void GUIWidget::registerArea(GUIArea* area)
	{
		assert(area != nullptr);

		mAreas.push_back(area);

		mWidgetIsDirty = true;
	}

	void GUIWidget::unregisterArea(GUIArea* area)
	{
		assert(area != nullptr);

		auto iterFind = std::find(begin(mAreas), end(mAreas), area);

		if(iterFind == mAreas.end())
			CM_EXCEPT(InvalidParametersException, "Cannot unregister an area that is not registered on this widget.");

		mAreas.erase(iterFind);
		mWidgetIsDirty = true;
	}

	void GUIWidget::setSkin(const GUISkin& skin)
	{
		mSkin = &skin;
	}

	const GUISkin& GUIWidget::getSkin() const
	{
		if(mSkin != nullptr)
			return *mSkin;
		else
			return DefaultSkin;
	}

	bool GUIWidget::isDirty(bool cleanIfDirty)
	{
		if(cleanIfDirty)
		{
			bool dirty = mWidgetIsDirty;
			mWidgetIsDirty = false;

			for(auto& elem : mElements)
			{
				if(elem->_isContentDirty())
				{
					dirty = true;
					elem->updateRenderElements();
				}

				if(elem->_isMeshDirty())
				{
					dirty = true;
					elem->_markAsClean();
				}
			}

			if(dirty)
				updateBounds();

			return dirty;
		}
		else
		{
			if(mWidgetIsDirty)
				return true;

			for(auto& elem : mElements)
			{
				if(elem->_isContentDirty() || elem->_isMeshDirty())
				{
					return true;
				}
			}

			return false;
		}
	}

	bool GUIWidget::inBounds(const Vector2I& position) const
	{
		// Technically GUI widget bounds can be larger than the viewport, so make sure we clip to viewport first
		if(!getTarget()->getArea().contains(position))
			return false;

		const Matrix4& worldTfrm = SO()->getWorldTfrm();
		Vector3 vecPos((float)position.x, (float)position.y, 0.0f);
		vecPos = worldTfrm.inverse().multiply3x4(vecPos);

		Vector2I localPos(Math::roundToInt(vecPos.x), Math::roundToInt(vecPos.y));
		return mBounds.contains(localPos);
	}

	void GUIWidget::updateBounds() const
	{
		if(mElements.size() > 0)
			mBounds = mElements[0]->_getClippedBounds();

		for(auto& elem : mElements)
		{
			RectI elemBounds = elem->_getClippedBounds();
			mBounds.encapsulate(elemBounds);
		}
	}

	void GUIWidget::ownerTargetResized()
	{
		for(auto& area : mAreas)
		{
			area->updateSizeBasedOnParent(getTarget()->getWidth(), getTarget()->getHeight());
		}
	}

	void GUIWidget::ownerWindowFocusChanged()
	{

	}
}