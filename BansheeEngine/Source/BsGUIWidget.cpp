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
		if (mPanel->_isContentDirty())
		{
			Rect2I clipRect(0, 0, getTarget()->getWidth(), getTarget()->getHeight());
			mPanel->_updateLayout(0, 0, getTarget()->getWidth(), getTarget()->getHeight(), clipRect,
				getDepth(), 0, -1);
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

		if (iterFind != mElements.end())
		{
			mElements.erase(iterFind);
			mWidgetIsDirty = true;
		}
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
		if(cleanIfDirty)
		{
			bool dirty = mWidgetIsDirty;
			mWidgetIsDirty = false;

			for(auto& elem : mElements)
			{
				if(elem->_isContentDirty())
				{
					dirty = true;
					elem->_updateRenderElements();
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
		mPanel->setWidth(getTarget()->getWidth());
		mPanel->setHeight(getTarget()->getHeight());
	}

	void GUIWidget::ownerWindowFocusChanged()
	{

	}
}