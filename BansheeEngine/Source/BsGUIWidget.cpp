#include "BsGUIWidget.h"
#include "BsGUIManager.h"
#include "BsGUISkin.h"
#include "BsGUILabel.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIArea.h"
#include "CmApplication.h"
#include "CmDeferredRenderContext.h"
#include "CmMaterial.h"
#include "CmPass.h"
#include "CmMesh.h"
#include "CmInt2.h"
#include "BsOverlayManager.h"
#include "BsCamera.h"
#include "CmViewport.h"
#include "CmSceneObject.h"
#include "CmRenderWindow.h"

using namespace CamelotFramework;

namespace BansheeEngine
{
	GUISkin GUIWidget::DefaultSkin;

	GUIWidget::GUIWidget(const HSceneObject& parent)
		:Component(parent), mSkin(nullptr), mOwnerWindow(nullptr), mWidgetIsDirty(false), mTarget(nullptr), mDepth(0)
	{	}

	GUIWidget::~GUIWidget()
	{
		if(mTarget != nullptr)
			GUIManager::instance().unregisterWidget(this);

		for(auto& elem : mElements)
		{
			GUIElement::_destroyInternal(elem);
		}

		for(auto& area : mAreas)
		{
			GUIArea::destroyInternal(area);
		}

		mElements.clear();
	}

	void GUIWidget::initialize(Viewport* target, const RenderWindow* ownerWindow)
	{
		assert(target != nullptr);
		assert(ownerWindow != nullptr);

		if(mOwnerWindow != nullptr)
			CM_EXCEPT(InvalidStateException, "Widget has already been initialized.");

		mTarget = target;
		mOwnerWindow = ownerWindow;

		GUIManager::instance().registerWidget(this);

		mOwnerWindow->onWindowMovedOrResized.connect(boost::bind(&GUIWidget::ownerWindowResized, this, _1));
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

	void GUIWidget::setSkin(const GUISkin* skin)
	{
		mSkin = skin;
	}

	const GUISkin* GUIWidget::getGUISkin() const
	{
		if(mSkin != nullptr)
			return mSkin;
		else
			return &DefaultSkin;
	}

	bool GUIWidget::isDirty(bool cleanIfDirty)
	{
		if(cleanIfDirty)
		{
			bool dirty = mWidgetIsDirty;
			mWidgetIsDirty = false;

			for(auto& elem : mElements)
			{
				if(elem->_isDirty())
				{
					dirty = true;
					elem->updateRenderElements();
				}
			}

			updateBounds();
			return dirty;
		}
		else
		{
			if(mWidgetIsDirty)
				return true;

			for(auto& elem : mElements)
			{
				if(elem->_isDirty())
				{
					return true;
				}
			}

			return false;
		}
	}

	bool GUIWidget::inBounds(const Int2& position) const
	{
		// Technically GUI widget bounds can be larger than the viewport, so make sure we clip to viewport first
		if(!getTarget()->getDimensions().contains(position))
			return false;

		const Matrix4& worldTfrm = SO()->getWorldTfrm();
		Vector3 vecPos((float)position.x, (float)position.y, 0.0f);
		vecPos = worldTfrm.inverse() * vecPos;

		Int2 localPos(Math::RoundToInt(vecPos.x), Math::RoundToInt(vecPos.y));
		return mBounds.contains(localPos);
	}

	void GUIWidget::updateBounds() const
	{
		if(mElements.size() > 0)
			mBounds = mElements[0]->_getBounds();

		for(auto& elem : mElements)
		{
			Rect elemBounds = elem->_getBounds();
			mBounds.encapsulate(elemBounds);
		}
	}

	void GUIWidget::ownerWindowResized(RenderWindow* window)
	{
		for(auto& area : mAreas)
		{
			area->notifyWindowResized(window->getWidth(), window->getHeight());
		}
	}
}