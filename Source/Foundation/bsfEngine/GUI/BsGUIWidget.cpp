//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "GUI/BsGUIWidget.h"
#include "GUI/BsGUIManager.h"
#include "GUI/BsGUISkin.h"
#include "GUI/BsGUILabel.h"
#include "GUI/BsGUIPanel.h"
#include "GUI/BsGUINavGroup.h"
#include "Math/BsVector2I.h"
#include "Components/BsCCamera.h"
#include "RenderAPI/BsViewport.h"
#include "Scene/BsSceneObject.h"
#include "Resources/BsBuiltinResources.h"

namespace bs
{
	GUIWidget::GUIWidget(const SPtr<Camera>& camera)
		: mCamera(camera)
	{
		construct(camera);
	}

	GUIWidget::GUIWidget(const HCamera& camera)
		: mCamera(camera->_getCamera())
	{
		construct(mCamera);
	}

	void GUIWidget::construct(const SPtr<Camera>& camera)
	{
		if (mCamera != nullptr)
		{
			SPtr<RenderTarget> target = mCamera->getViewport()->getTarget();

			if (target != nullptr)
				mCachedRTId = target->getInternalID();
		}

		mDefaultNavGroup = GUINavGroup::create();

		GUIManager::instance().registerWidget(this);

		mPanel = GUIPanel::create();
		mPanel->_changeParentWidget(this);
		updateRootPanel();

		GUIDrawGroup mainDrawGroup;
		mainDrawGroup.minDepth = 0;
		mainDrawGroup.depthRange = std::numeric_limits<UINT32>::max();
		mainDrawGroup.id = mNextDrawGroupId++;
		
		mDrawGroups.push_back(mainDrawGroup);
	}

	GUIWidget::~GUIWidget()
	{
		_destroy();
	}

	SPtr<GUIWidget> GUIWidget::create(const SPtr<Camera>& camera)
	{
		return bs_shared_ptr(new (bs_alloc<GUIWidget>()) GUIWidget(camera));
	}

	SPtr<GUIWidget> GUIWidget::create(const HCamera& camera)
	{
		return bs_shared_ptr(new (bs_alloc<GUIWidget>()) GUIWidget(camera));
	}

	void GUIWidget::_destroy()
	{
		if (mPanel != nullptr)
		{
			GUILayout::destroy(mPanel);
			mPanel = nullptr;
		}

		if (mCamera != nullptr)
		{
			GUIManager::instance().unregisterWidget(this);
			mCamera = nullptr;
		}

		mElements.clear();
		mDirtyContents.clear();
	}

	void GUIWidget::setDepth(UINT8 depth)
	{
		mDepth = depth;
		mWidgetIsDirty = true;

		updateRootPanel();
	}

	Viewport* GUIWidget::getTarget() const
	{
		if(mCamera != nullptr)
			return mCamera->getViewport().get();

		return nullptr;
	}

	void GUIWidget::_updateTransform(const HSceneObject& parent)
	{
		// If the widgets parent scene object moved, we need to mark it as dirty
		// as the GUIManager batching relies on object positions, so it needs to be updated.
		const float diffEpsilon = 0.0001f;

		const Transform& tfrm = parent->getTransform();
		Vector3 position = tfrm.getPosition();
		Quaternion rotation = tfrm.getRotation();
		Vector3 scale = tfrm.getScale();

		if(!mWidgetIsDirty)
		{
			if(!Math::approxEquals(mPosition, position, diffEpsilon))
				mWidgetIsDirty = true;
			else
			{
				if(!Math::approxEquals(mRotation, rotation, diffEpsilon))
					mWidgetIsDirty = true;
				else
				{
					if(Math::approxEquals(mScale, scale))
						mWidgetIsDirty = true;
				}
			}
		}

		mPosition = position;
		mRotation = rotation;
		mScale = scale;
		mTransform = parent->getWorldMatrix();
	}

	void GUIWidget::_updateRT()
	{
		SPtr<RenderTarget> rt;
		UINT64 newRTId = 0;
		if(mCamera != nullptr)
		{
			rt = mCamera->getViewport()->getTarget();
			if (rt != nullptr)
				newRTId = rt->getInternalID();
		}

		if(mCachedRTId != newRTId)
		{
			mCachedRTId = newRTId;
			updateRootPanel();
		}
	}

	void GUIWidget::_updateLayout()
	{
		// Check if render target size changed and update if needed
		// Note: Purposely not relying to the RenderTarget::onResized callback, as it will trigger /before/ Input events.
		// These events might trigger a resize, meaning the size would be delayed one frame, resulting in a visual artifact
		// where the GUI doesn't match the target size.
		Viewport* target = getTarget();
		if (target != nullptr)
		{
			Rect2I area = target->getPixelArea();
			UINT32 width = area.width;
			UINT32 height = area.height;

			const Rect2I& panelArea = mPanel->_getLayoutData().area;
			if(panelArea.width != width || panelArea.height != height)
			{
				updateRootPanel();
				onOwnerTargetResized();
			}
		}

		bs_frame_mark();

		// Determine dirty contents and layouts
		FrameStack<GUIElementBase*> todo;
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
					_updateLayout(updateParent);
				else // Must be root panel
					_updateLayout(mPanel);
			}
			else
			{
				UINT32 numChildren = currentElem->_getNumChildren();
				for (UINT32 i = 0; i < numChildren; i++)
					todo.push(currentElem->_getChild(i));
			}
		}

		bs_frame_clear();
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
			panel->_updateDepthRange(childLayoutData);
			childLayoutData.area = elementArea;

			panel->_updateChildLayout(dirtyElement, childLayoutData);
		}
		else
		{
			GUILayoutData childLayoutData = updateParent->_getLayoutData();
			updateParent->_updateLayout(childLayoutData);
		}
		
		// Mark dirty contents
		bs_frame_mark();
		{
			FrameStack<GUIElementBase*> todo;
			todo.push(elem);

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
		bs_frame_clear();
	}

	GUIWidget::GUIDrawGroup& GUIWidget::splitDrawGroup(UINT32 groupIdx, UINT32 depth)
	{
		GUIDrawGroup& group = mDrawGroups[groupIdx];
		assert(depth > group.minDepth);
		
		UINT32 maxDepth = group.minDepth + group.depthRange;
		group.depthRange = depth - group.minDepth;

		GUIDrawGroup newSplitGroup;
		newSplitGroup.minDepth = depth;
		newSplitGroup.depthRange = maxDepth - newSplitGroup.minDepth + 1;
		newSplitGroup.id = mNextDrawGroupId++;

		auto it = std::partition(group.cachedElements.begin(), group.cachedElements.end(),
			[depth](const GUIGroupElement& x)
		{
				UINT32 elemDepth = x.element->_getRenderElementDepth(x.renderElement);
				return elemDepth < depth;
		});

		std::move(it, group.cachedElements.end(), std::back_inserter(newSplitGroup.cachedElements));
		group.cachedElements.erase(it, group.cachedElements.end());

		for (auto& entry : newSplitGroup.cachedElements)
			entry.element->_setDrawGroupId(newSplitGroup.id);

		updateDrawGroupBounds(group);
		updateDrawGroupBounds(newSplitGroup);

		mDrawGroups.insert(mDrawGroups.begin() + groupIdx, std::move(newSplitGroup));
		return mDrawGroups[groupIdx + 1];
	}

	void GUIWidget::updateDrawGroupBounds(GUIDrawGroup& group)
	{
		group.bounds = Rect2I();

		for(auto& entry : group.cachedElements)
		{
			Rect2I tfrmedBounds = entry.element->_getClippedBounds();
			tfrmedBounds.transform(getWorldTfrm());
			
			group.bounds.encapsulate(tfrmedBounds);
		}
	}

	void GUIWidget::_registerElement(GUIElementBase* elem)
	{
		assert(elem != nullptr && !elem->_isDestroyed());

		if (elem->_getType() == GUIElementBase::Type::Element)
		{
			mElements.push_back(static_cast<GUIElement*>(elem));
			mWidgetIsDirty = true;

			// Find a draw group
			auto guiElem = static_cast<GUIElement*>(elem);
			UINT32 numRenderElems = guiElem->_getNumRenderElements();

			for(UINT32 i = 0; i < numRenderElems; i++)
			{
				UINT32 elemDepth = guiElem->_getRenderElementDepth(i);

				Rect2I tfrmedBounds = guiElem->_getClippedBounds();
				tfrmedBounds.transform(getWorldTfrm());

				SpriteMaterial* spriteMaterial = nullptr;
				guiElem->_getMaterial(i, &spriteMaterial);
				assert(spriteMaterial != nullptr);

				// Groups are expected to be sorted by minDepth
				for(UINT32 j = 0; j < (UINT32)mDrawGroups.size(); j++)
				{
					GUIDrawGroup& group = mDrawGroups[j];
					if (elemDepth < group.minDepth)
						continue;

					if (spriteMaterial->allowBatching())
					{
						group.cachedElements.push_back(GUIGroupElement(guiElem, i));
						group.bounds.encapsulate(tfrmedBounds);
						guiElem->_setDrawGroupId(group.id);
					}
					else
					{
						bool needsSplit = elemDepth != group.minDepth;
						if(needsSplit)
						{
							GUIDrawGroup& newGroup = splitDrawGroup(j, elemDepth);

							newGroup.nonCachedElements.push_back(GUIGroupElement(guiElem, i));
							guiElem->_setDrawGroupId(newGroup.id);
						}
						else
						{
							group.nonCachedElements.push_back(GUIGroupElement(guiElem, i));
							guiElem->_setDrawGroupId(group.id);
						}
					}
					
					break;
				}
			}
		}
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
		{
			mDirtyContents.erase(static_cast<GUIElement*>(elem));

			auto guiElem = static_cast<GUIElement*>(elem);
			UINT32 drawGroupId = guiElem->_getDrawGroupId();
			auto iterFind2 = std::find_if(mDrawGroups.begin(), mDrawGroups.end(),
				[drawGroupId](const GUIDrawGroup& group) { return group.id == drawGroupId; });

			assert(iterFind2 != mDrawGroups.end());
			if(iterFind2 != mDrawGroups.end())
			{
				GUIDrawGroup& group = *iterFind2;

				group.cachedElements.erase(std::remove_if(group.cachedElements.begin(), group.cachedElements.end(),
					[guiElem](const GUIGroupElement& x) { return x.element == guiElem; }), group.cachedElements.end());
				
				group.nonCachedElements.erase(std::remove_if(group.nonCachedElements.begin(), group.nonCachedElements.end(),
					[guiElem](const GUIGroupElement& x) { return x.element == guiElem; }), group.nonCachedElements.end());

				// Attempt to merge with previous group
				if(group.nonCachedElements.empty() && group.minDepth > 0)
				{
					assert(iterFind2 != mDrawGroups.begin());

					auto prevGroupIter = iterFind2 - 1;
					GUIDrawGroup& prevGroup = *prevGroupIter;

					prevGroup.depthRange += group.depthRange;

					for (auto& entry : group.cachedElements)
						entry.element->_setDrawGroupId(prevGroup.id);
					
					std::move(group.cachedElements.begin(), group.cachedElements.end(), std::back_inserter(prevGroup.cachedElements));
					updateDrawGroupBounds(prevGroup);

					mDrawGroups.erase(iterFind2);
				}
			}
		}
	}

	void GUIWidget::_markMeshDirty(GUIElementBase* elem)
	{
		mWidgetIsDirty = true;
	}

	void GUIWidget::_markContentDirty(GUIElementBase* elem)
	{
		if (elem->_getType() == GUIElementBase::Type::Element)
			mDirtyContents.insert(static_cast<GUIElement*>(elem));
	}

	void GUIWidget::setSkin(const HGUISkin& skin)
	{
		mSkin = skin;

		for(auto& element : mElements)
			element->_refreshStyle();
	}

	const GUISkin& GUIWidget::getSkin() const
	{
		if(mSkin.isLoaded())
			return *mSkin;
		else
			return *BuiltinResources::instance().getGUISkin();
	}

	void GUIWidget::setCamera(const SPtr<Camera>& camera)
	{
		SPtr<Camera> newCamera = camera;
		if(newCamera != nullptr)
		{
			if (newCamera->getViewport()->getTarget() == nullptr)
				newCamera = nullptr;
		}

		if (mCamera == newCamera)
			return;

		GUIManager::instance().unregisterWidget(this);
		mCamera = newCamera;
		GUIManager::instance().registerWidget(this);

		updateRootPanel();
	}

	void GUIWidget::setIsActive(bool active)
	{
		mIsActive = active;
	}

	bool GUIWidget::isDirty(bool cleanIfDirty)
	{
		if (!mIsActive)
			return false;

		const bool dirty = mWidgetIsDirty || !mDirtyContents.empty();

		if(cleanIfDirty && dirty)
		{
			mWidgetIsDirty = false;

			// Update render contents recursively because updates can cause child GUI elements to become dirty
			while(!mDirtyContents.empty())
			{
				mDirtyContentsTemp.swap(mDirtyContents);

				for (auto& dirtyElement : mDirtyContentsTemp)
					dirtyElement->_updateRenderElements();

				mDirtyContentsTemp.clear();
			}

			updateBounds();
		}
		
		return dirty;
	}

	bool GUIWidget::inBounds(const Vector2I& position) const
	{
		Viewport* target = getTarget();
		if (target == nullptr)
			return false;

		// Technically GUI widget bounds can be larger than the viewport, so make sure we clip to viewport first
		if(!target->getPixelArea().contains(position))
			return false;

		Vector3 vecPos((float)position.x, (float)position.y, 0.0f);
		vecPos = mTransform.inverse().multiplyAffine(vecPos);

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

	void GUIWidget::ownerWindowFocusChanged()
	{
		onOwnerWindowFocusChanged();
	}

	void GUIWidget::updateRootPanel()
	{
		Viewport* target = getTarget();
		if (target == nullptr)
			return;

		Rect2I area = target->getPixelArea();
		UINT32 width = area.width;
		UINT32 height = area.height;

		GUILayoutData layoutData;
		layoutData.area.width = width;
		layoutData.area.height = height;
		layoutData.clipRect = Rect2I(0, 0, width, height);
		layoutData.setWidgetDepth(mDepth);

		mPanel->setWidth(width);
		mPanel->setHeight(height);

		mPanel->_setLayoutData(layoutData);
		mPanel->_markLayoutAsDirty();
	}
}
