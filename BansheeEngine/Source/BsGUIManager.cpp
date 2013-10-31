#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUIElement.h"
#include "BsImageSprite.h"
#include "BsSpriteTexture.h"
#include "CmTime.h"
#include "CmSceneObject.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"
#include "CmRenderWindowManager.h"
#include "CmPlatform.h"
#include "CmRect.h"
#include "CmApplication.h"
#include "CmException.h"
#include "CmInput.h"
#include "CmPass.h"
#include "CmDebug.h"
#include "CmRenderQueue.h"
#include "BsGUIInputCaret.h"
#include "BsGUIInputSelection.h"
#include "BsGUIListBox.h"
#include "BsGUIButton.h"
#include "BsGUIDropDownBox.h"
#include "BsGUIContextMenu.h"
#include "BsDragAndDropManager.h"
#include "BsGUIDropDownBoxManager.h"
#include "BsGUIContextMenu.h"
#include "CmProfiler.h"

using namespace CamelotFramework;
namespace BansheeEngine
{
	struct GUIGroupElement
	{
		GUIGroupElement()
		{ }

		GUIGroupElement(GUIElement* _element, UINT32 _renderElement)
			:element(_element), renderElement(_renderElement)
		{ }

		GUIElement* element;
		UINT32 renderElement;
	};

	struct GUIMaterialGroup
	{
		HMaterial material;
		UINT32 numQuads;
		UINT32 depth;
		Rect bounds;
		Vector<GUIGroupElement>::type elements;
	};

	UINT32 GUIManager::DRAG_DISTANCE = 3;

	GUIManager::GUIManager()
		:mElementUnderCursor(nullptr), mWidgetUnderCursor(nullptr), mSeparateMeshesByWidget(true), mActiveElement(nullptr), 
		mActiveWidget(nullptr), mActiveMouseButton(GUIMouseButton::Left), mKeyboardFocusElement(nullptr), mKeyboardFocusWidget(nullptr),
		mCaretTexture(nullptr), mCaretBlinkInterval(0.5f), mCaretLastBlinkTime(0.0f), mCaretColor(1.0f, 0.6588f, 0.0f), mIsCaretOn(false),
		mTextSelectionColor(1.0f, 0.6588f, 0.0f), mInputCaret(nullptr), mInputSelection(nullptr), mSelectiveInputActive(false), mDragState(DragState::NoDrag)
	{
		mOnCursorMovedConn = gInput().onCursorMoved.connect(boost::bind(&GUIManager::onCursorMoved, this, _1));
		mOnCursorPressedConn = gInput().onCursorPressed.connect(boost::bind(&GUIManager::onCursorPressed, this, _1));
		mOnCursorReleasedConn = gInput().onCursorReleased.connect(boost::bind(&GUIManager::onCursorReleased, this, _1));
		mOnCursorDoubleClick = gInput().onDoubleClick.connect(boost::bind(&GUIManager::onCursorDoubleClick, this, _1));
		mOnTextInputConn = gInput().onCharInput.connect(boost::bind(&GUIManager::onTextInput, this, _1)); 
		mOnInputCommandConn = gInput().onInputCommand.connect(boost::bind(&GUIManager::onInputCommandEntered, this, _1)); 

		mWindowGainedFocusConn = RenderWindowManager::instance().onFocusGained.connect(boost::bind(&GUIManager::onWindowFocusGained, this, _1));
		mWindowLostFocusConn = RenderWindowManager::instance().onFocusLost.connect(boost::bind(&GUIManager::onWindowFocusLost, this, _1));

		mMouseLeftWindowConn = Platform::onMouseLeftWindow.connect(boost::bind(&GUIManager::onMouseLeftWindow, this, _1));

		mInputCaret = cm_new<GUIInputCaret, PoolAlloc>();
		mInputSelection = cm_new<GUIInputSelection, PoolAlloc>();

		DragAndDropManager::startUp(cm_new<DragAndDropManager>());
		mDragEndedConn = DragAndDropManager::instance().onDragEnded.connect(boost::bind(&GUIManager::onMouseDragEnded, this, _1));

		GUIDropDownBoxManager::startUp(cm_new<GUIDropDownBoxManager>());

		// Need to defer this call because I want to make sure all managers are initialized first
		deferredCall(std::bind(&GUIManager::updateCaretTexture, this));
		deferredCall(std::bind(&GUIManager::updateTextSelectionTexture, this));
	}

	GUIManager::~GUIManager()
	{
		GUIDropDownBoxManager::shutDown();
		DragAndDropManager::shutDown();

		// Make a copy of widgets, since destroying them will remove them from mWidgets and
		// we can't iterate over an array thats getting modified
		Vector<WidgetInfo>::type widgetCopy = mWidgets;
		for(auto& widget : widgetCopy)
			widget.widget->destroy();

		mOnCursorPressedConn.disconnect();
		mOnCursorReleasedConn.disconnect();
		mOnCursorMovedConn.disconnect();
		mOnCursorDoubleClick.disconnect();
		mOnTextInputConn.disconnect();
		mOnInputCommandConn.disconnect();

		mDragEndedConn.disconnect();

		mWindowGainedFocusConn.disconnect();
		mWindowLostFocusConn.disconnect();

		mMouseLeftWindowConn.disconnect();

		cm_delete<PoolAlloc>(mInputCaret);
		cm_delete<PoolAlloc>(mInputSelection);
	}

	void GUIManager::registerWidget(GUIWidget* widget)
	{
		mWidgets.push_back(WidgetInfo(widget));

		const Viewport* renderTarget = widget->getTarget();

		auto findIter = mCachedGUIData.find(renderTarget);

		if(findIter == end(mCachedGUIData))
			mCachedGUIData[renderTarget] = GUIRenderData();

		GUIRenderData& windowData = mCachedGUIData[renderTarget];
		windowData.widgets.push_back(widget);
		windowData.isDirty = true;
	}

	void GUIManager::unregisterWidget(GUIWidget* widget)
	{
		auto findIter = std::find_if(begin(mWidgets), end(mWidgets), [=] (const WidgetInfo& x) { return x.widget == widget; } );

		if(findIter != end(mWidgets))
		{
			mWidgets.erase(findIter);
		}

		if(mWidgetUnderCursor == widget)
		{
			mWidgetUnderCursor = nullptr;
			mElementUnderCursor = nullptr;
		}

		if(mKeyboardFocusWidget == widget)
		{
			mKeyboardFocusWidget = nullptr;
			mKeyboardFocusElement = nullptr;
		}

		if(mActiveWidget == widget)
		{
			mActiveWidget = nullptr;
			mActiveElement = nullptr;
		}

		const Viewport* renderTarget = widget->getTarget();
		GUIRenderData& renderData = mCachedGUIData[renderTarget];

		auto findIter2 = std::find(begin(renderData.widgets), end(renderData.widgets), widget);
		if(findIter2 != end(renderData.widgets))
			renderData.widgets.erase(findIter2);

		if(renderData.widgets.size() == 0)
			mCachedGUIData.erase(renderTarget);
		else
			renderData.isDirty = true;
	}

	void GUIManager::update()
	{
		DragAndDropManager::instance().update();

		// Update layouts
		gProfiler().beginSample("UpdateLayout");
		for(auto& widgetInfo : mWidgets)
		{
			widgetInfo.widget->_updateLayout();
		}
		gProfiler().endSample("UpdateLayout");

		// Blink caret
		if(mKeyboardFocusElement != nullptr)
		{
			float curTime = gTime().getTime();

			if((curTime - mCaretLastBlinkTime) >= mCaretBlinkInterval)
			{
				mCaretLastBlinkTime = curTime;
				mIsCaretOn = !mIsCaretOn;

				mCommandEvent = GUICommandEvent();
				mCommandEvent.setType(GUICommandEventType::Redraw);

				sendCommandEvent(mKeyboardFocusWidget, mKeyboardFocusElement, mCommandEvent);
			}
		}

		PROFILE_CALL(updateMeshes(), "UpdateMeshes");

		if(mElementUnderCursor != nullptr && mElementUnderCursor->_isDestroyed())
		{
			mElementUnderCursor = nullptr;
			mWidgetUnderCursor = nullptr;
		}

		if(mActiveElement != nullptr && mActiveElement->_isDestroyed())
		{
			mActiveElement = nullptr;
			mActiveWidget = nullptr;
		}

		if(mKeyboardFocusElement != nullptr && mKeyboardFocusElement->_isDestroyed())
		{
			mKeyboardFocusElement = nullptr;
			mKeyboardFocusWidget = nullptr;
		}

		processDestroyQueue();
	}

	void GUIManager::render(ViewportPtr& target, CM::RenderQueue& renderQueue) const
	{
		auto findIter = mCachedGUIData.find(target.get());

		if(findIter == mCachedGUIData.end())
			return;

		const GUIRenderData& renderData = findIter->second;

		// Render the meshes
		if(mSeparateMeshesByWidget)
		{
			// TODO - Possible optimization. I currently divide by width/height inside the shader, while it
			// might be more optimal to just scale the mesh as the resolution changes?
			float invViewportWidth = 1.0f / (target->getWidth() * 0.5f);
			float invViewportHeight = 1.0f / (target->getHeight() * 0.5f);

			UINT32 meshIdx = 0;
			for(auto& mesh : renderData.cachedMeshes)
			{
				HMaterial material = renderData.cachedMaterials[meshIdx];
				GUIWidget* widget = renderData.cachedWidgetsPerMesh[meshIdx];

				if(material == nullptr || !material.isLoaded())
				{
					meshIdx++;
					continue;
				}

				if(mesh == nullptr || !mesh.isLoaded())
				{
					meshIdx++;
					continue;
				}

				material->setFloat("invViewportWidth", invViewportWidth);
				material->setFloat("invViewportHeight", invViewportHeight);
				material->setMat4("worldTransform", widget->SO()->getWorldTfrm());

				renderQueue.add(material, mesh, 0, Vector3::ZERO);

				meshIdx++;
			}
		}
		else
		{
			// TODO: I want to avoid separating meshes by widget in the future. On DX11 and GL I can set up a shader
			// that accepts multiple world transforms (one for each widget). Then I can add some instance information to vertices
			// and render elements using multiple different transforms with a single call.
			// Separating meshes can then be used as a compatibility mode for DX9

			CM_EXCEPT(NotImplementedException, "Not implemented");
		}
	}

	void GUIManager::updateMeshes()
	{
		for(auto& cachedMeshData : mCachedGUIData)
		{
			GUIRenderData& renderData = cachedMeshData.second;

			// Check if anything is dirty. If nothing is we can skip the update
			bool isDirty = renderData.isDirty;
			renderData.isDirty = false;

			for(auto& widget : renderData.widgets)
			{
				if(widget->isDirty(true))
				{
					isDirty = true;
				}
			}

			if(!isDirty)
				continue;

			// Make a list of all GUI elements, sorted from farthest to nearest (highest depth to lowest)
			auto elemComp = [](const GUIGroupElement& a, const GUIGroupElement& b)
			{
				UINT32 aDepth = a.element->_getRenderElementDepth(a.renderElement);
				UINT32 bDepth = b.element->_getRenderElementDepth(b.renderElement);

				// Compare pointers just to differentiate between two elements with the same depth, their order doesn't really matter, but std::set
				// requires all elements to be unique
				return (aDepth > bDepth) || 
					(aDepth == bDepth && a.element > b.element) || 
					(aDepth == bDepth && a.element == b.element && a.renderElement > b.renderElement); 
			};

			Set<GUIGroupElement, std::function<bool(const GUIGroupElement&, const GUIGroupElement&)>>::type allElements(elemComp);

			for(auto& widget : renderData.widgets)
			{
				const Vector<GUIElement*>::type& elements = widget->getElements();

				for(auto& element : elements)
				{
					if(element->_isDisabled())
						continue;

					UINT32 numRenderElems = element->getNumRenderElements();
					for(UINT32 i = 0; i < numRenderElems; i++)
					{
						allElements.insert(GUIGroupElement(element, i));
					}
				}
			}

			// Group the elements in such a way so that we end up with a smallest amount of
			// meshes, without breaking back to front rendering order
			UnorderedMap<UINT64, Vector<GUIMaterialGroup>::type>::type materialGroups;
			for(auto& elem : allElements)
			{
				GUIElement* guiElem = elem.element;
				UINT32 renderElemIdx = elem.renderElement;
				UINT32 elemDepth = guiElem->_getRenderElementDepth(renderElemIdx);

				Rect tfrmedBounds = guiElem->_getClippedBounds();
				tfrmedBounds.transform(guiElem->_getParentWidget().SO()->getWorldTfrm());

				const HMaterial& mat = guiElem->getMaterial(renderElemIdx);

				UINT64 materialId = mat->getInternalID(); // TODO - I group based on material ID. So if two widgets used exact copies of the same material
				// this system won't detect it. Find a better way of determining material similarity?

				// If this is a new material, add a new list of groups
				auto findIterMaterial = materialGroups.find(materialId);
				if(findIterMaterial == end(materialGroups))
					materialGroups[materialId] = Vector<GUIMaterialGroup>::type();

				// Try to find a group this material will fit in:
				//  - Group that has a depth value same or one below elements depth will always be a match
				//  - Otherwise, we search higher depth values as well, but we only use them if no elements in between those depth values
				//    overlap the current elements bounds.
				Vector<GUIMaterialGroup>::type& allGroups = materialGroups[materialId];
				GUIMaterialGroup* foundGroup = nullptr;
				for(auto groupIter = allGroups.rbegin(); groupIter != allGroups.rend(); ++groupIter)
				{
					// If we separate meshes by widget, ignore any groups with widget parents other than mine
					if(mSeparateMeshesByWidget)
					{
						if(groupIter->elements.size() > 0)
						{
							GUIElement* otherElem = groupIter->elements.begin()->element; // We only need to check the first element
							if(&otherElem->_getParentWidget() != &guiElem->_getParentWidget())
								continue;
						}
					}

					GUIMaterialGroup& group = *groupIter;

					if(group.depth == elemDepth || group.depth == (elemDepth - 1))
					{
						foundGroup = &group;
						break;
					}
					else
					{
						UINT32 startDepth = elemDepth;
						UINT32 endDepth = group.depth;

						bool foundOverlap = false;
						for(auto& material : materialGroups)
						{
							for(auto& group : material.second)
							{
								if(group.depth > startDepth && group.depth < endDepth)
								{
									if(group.bounds.overlaps(tfrmedBounds))
									{
										foundOverlap = true;
										break;
									}
								}
							}
						}

						if(!foundOverlap)
						{
							foundGroup = &group;
							break;
						}
					}
				}

				if(foundGroup == nullptr)
				{
					allGroups.push_back(GUIMaterialGroup());
					foundGroup = &allGroups[allGroups.size() - 1];

					foundGroup->depth = elemDepth;
					foundGroup->bounds = tfrmedBounds;
					foundGroup->elements.push_back(GUIGroupElement(guiElem, renderElemIdx));
					foundGroup->material = mat;
					foundGroup->numQuads = guiElem->getNumQuads(renderElemIdx);
				}
				else
				{
					foundGroup->bounds.encapsulate(tfrmedBounds);
					foundGroup->elements.push_back(GUIGroupElement(guiElem, renderElemIdx));
					foundGroup->numQuads += guiElem->getNumQuads(renderElemIdx);
				}
			}

			// Make a list of all GUI elements, sorted from farthest to nearest (highest depth to lowest)
			auto groupComp = [](GUIMaterialGroup* a, GUIMaterialGroup* b)
			{
				return (a->depth > b->depth) || (a->depth == b->depth && a > b);
				// Compare pointers just to differentiate between two elements with the same depth, their order doesn't really matter, but std::set
				// requires all elements to be unique
			};

			Set<GUIMaterialGroup*, std::function<bool(GUIMaterialGroup*, GUIMaterialGroup*)>>::type sortedGroups(groupComp);
			for(auto& material : materialGroups)
			{
				for(auto& group : material.second)
				{
					sortedGroups.insert(&group);
				}
			}

			UINT32 numMeshes = (UINT32)sortedGroups.size();
			UINT32 oldNumMeshes = (UINT32)renderData.cachedMeshes.size();
			if(numMeshes < oldNumMeshes)
				renderData.cachedMeshes.resize(numMeshes);

			renderData.cachedMaterials.resize(numMeshes);

			if(mSeparateMeshesByWidget)
				renderData.cachedWidgetsPerMesh.resize(numMeshes);

			// Fill buffers for each group and update their meshes
			UINT32 groupIdx = 0;
			for(auto& group : sortedGroups)
			{
				renderData.cachedMaterials[groupIdx] = group->material;

				if(mSeparateMeshesByWidget)
				{
					if(group->elements.size() == 0)
						renderData.cachedWidgetsPerMesh[groupIdx] = nullptr;
					else
					{
						GUIElement* elem = group->elements.begin()->element;
						renderData.cachedWidgetsPerMesh[groupIdx] = &elem->_getParentWidget();
					}
				}

				MeshDataPtr meshData = cm_shared_ptr<MeshData, PoolAlloc>(group->numQuads * 4);

				meshData->beginDesc();
				meshData->addVertElem(VET_FLOAT2, VES_POSITION);
				meshData->addVertElem(VET_FLOAT2, VES_TEXCOORD);
				meshData->addSubMesh(group->numQuads * 6);
				meshData->endDesc();

				UINT8* vertices = meshData->getElementData(VES_POSITION);
				UINT8* uvs = meshData->getElementData(VES_TEXCOORD);
				UINT32* indices = meshData->getIndices32();
				UINT32 vertexStride = meshData->getVertexStride();
				UINT32 indexStride = meshData->getIndexElementSize();

				UINT32 quadOffset = 0;
				for(auto& matElement : group->elements)
				{
					matElement.element->fillBuffer(vertices, uvs, indices, quadOffset, group->numQuads, vertexStride, indexStride, matElement.renderElement);

					UINT32 numQuads = matElement.element->getNumQuads(matElement.renderElement);
					UINT32 indexStart = quadOffset * 6;
					UINT32 indexEnd = indexStart + numQuads * 6;
					UINT32 vertOffset = quadOffset * 4;

					for(UINT32 i = indexStart; i < indexEnd; i++)
						indices[i] += vertOffset;

					quadOffset += numQuads;
				}

				if(groupIdx >= (UINT32)renderData.cachedMeshes.size())
				{
					renderData.cachedMeshes.push_back(Mesh::create());
				}

				gMainCA().writeSubresource(renderData.cachedMeshes[groupIdx].getInternalPtr(), 0, meshData);

				groupIdx++;
			}
		}
	}

	void GUIManager::updateCaretTexture()
	{
		if(mCaretTexture == nullptr)
		{
			HTexture newTex = Texture::create(TEX_TYPE_2D, 1, 1, 0, PF_R8G8B8A8);
			mCaretTexture = cm_shared_ptr<SpriteTexture>(newTex);
		}

		const HTexture& tex = mCaretTexture->getTexture();
		UINT32 subresourceIdx = tex->mapToSubresourceIdx(0, 0);
		PixelDataPtr data = tex->allocateSubresourceBuffer(subresourceIdx);

		data->setColorAt(mCaretColor, 0, 0);

		gMainCA().writeSubresource(tex.getInternalPtr(), tex->mapToSubresourceIdx(0, 0), data);
	}

	void GUIManager::updateTextSelectionTexture()
	{
		if(mTextSelectionTexture == nullptr)
		{
			HTexture newTex = Texture::create(TEX_TYPE_2D, 1, 1, 0, PF_R8G8B8A8);
			mTextSelectionTexture = cm_shared_ptr<SpriteTexture>(newTex);
		}

		const HTexture& tex = mTextSelectionTexture->getTexture();
		UINT32 subresourceIdx = tex->mapToSubresourceIdx(0, 0);
		PixelDataPtr data = tex->allocateSubresourceBuffer(subresourceIdx);

		data->setColorAt(mTextSelectionColor, 0, 0);

		gMainCA().writeSubresource(tex.getInternalPtr(), tex->mapToSubresourceIdx(0, 0), data);
	}

	bool GUIManager::onMouseDragEnded(const CM::PositionalInputEvent& event)
	{
		GUIMouseButton guiButton = buttonToGUIButton(event.button);

		if(DragAndDropManager::instance().isDragInProgress() && guiButton == GUIMouseButton::Left)
		{
			if(mElementUnderCursor != nullptr)
			{
				mMouseEvent.setDragAndDropDroppedData(mElementUnderCursor, Int2(), DragAndDropManager::instance().getDragTypeId(), DragAndDropManager::instance().getDragData());
				bool processed = sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent);

				return processed;
			}
		}

		return false;
	}

	void GUIManager::onCursorMoved(const PositionalInputEvent& event)
	{
		if(event.isUsed())
			return;

		bool buttonStates[(int)GUIMouseButton::Count];
		buttonStates[0] = event.buttonStates[0];
		buttonStates[1] = event.buttonStates[1];
		buttonStates[2] = event.buttonStates[2];

		if(findElementUnderCursor(event.screenPos, buttonStates, event.shift, event.control, event.alt))
			event.markAsUsed();

		Int2 localPos;
		
		if(mWidgetUnderCursor != nullptr)
			localPos = getWidgetRelativePos(*mWidgetUnderCursor, event.screenPos);

		if(mActiveElement != nullptr && mDragState == DragState::HeldWithoutDrag)
		{
			UINT32 dist = mLastCursorClickPos.manhattanDist(event.screenPos);

			if(dist > DRAG_DISTANCE)
			{
				mMouseEvent.setMouseDragStartData(mElementUnderCursor, localPos);
				if(sendMouseEvent(mActiveWidget, mActiveElement, mMouseEvent))
					event.markAsUsed();

				mDragState = DragState::Dragging;
			}
		}

		// If mouse is being held down send MouseDrag events
		if(mActiveElement != nullptr && mDragState == DragState::Dragging)
		{
			Int2 curLocalPos = getWidgetRelativePos(*mActiveWidget, event.screenPos);

			if(mLastCursorLocalPos != curLocalPos)
			{
				mMouseEvent.setMouseDragData(mElementUnderCursor, curLocalPos, curLocalPos - mLastCursorLocalPos);
				if(sendMouseEvent(mActiveWidget, mActiveElement, mMouseEvent))
					event.markAsUsed();

				mLastCursorLocalPos = curLocalPos;
			}

			// Also if drag is in progress send DragAndDrop events
			if(DragAndDropManager::instance().isDragInProgress())
			{
				if(mElementUnderCursor != nullptr)
				{
					mMouseEvent.setDragAndDropDraggedData(mElementUnderCursor, localPos, DragAndDropManager::instance().getDragTypeId(), DragAndDropManager::instance().getDragData());
					if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
						event.markAsUsed();
				}
			}
		}
		else // Otherwise, send MouseMove events if we are hovering over any element
		{
			if(mElementUnderCursor != nullptr)
			{
				// Send MouseMove event
				if(mLastCursorLocalPos != localPos)
				{
					mMouseEvent.setMouseMoveData(mElementUnderCursor, localPos);
					if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
						event.markAsUsed();

					mLastCursorLocalPos = localPos;
				}

				if(Math::Abs(event.mouseWheelScrollAmount) > 0.00001f)
				{
					mMouseEvent.setMouseWheelScrollData(mElementUnderCursor, event.mouseWheelScrollAmount);
					if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
						event.markAsUsed();
				}
			}
		}
	}

	void GUIManager::onCursorReleased(const CM::PositionalInputEvent& event)
	{
		if(event.isUsed())
			return;

		bool buttonStates[(int)GUIMouseButton::Count];
		buttonStates[0] = event.buttonStates[0];
		buttonStates[1] = event.buttonStates[1];
		buttonStates[2] = event.buttonStates[2];

		if(findElementUnderCursor(event.screenPos, buttonStates, event.shift, event.control, event.alt))
			event.markAsUsed();

		mMouseEvent = GUIMouseEvent(buttonStates, event.shift, event.control, event.alt);

		Int2 localPos;
		if(mWidgetUnderCursor != nullptr)
		{
			localPos = getWidgetRelativePos(*mWidgetUnderCursor, event.screenPos);
		}

		GUIMouseButton guiButton = buttonToGUIButton(event.button);

		// Send MouseUp event only if we are over the active element (we don't want to accidentally trigger other elements).
		// And only activate when a button that originally caused the active state is released, otherwise ignore it.
		bool acceptMouseUp = mActiveMouseButton == guiButton && (mElementUnderCursor != nullptr && mActiveElement == mElementUnderCursor);
		if(acceptMouseUp)
		{
			mMouseEvent.setMouseUpData(mElementUnderCursor, localPos, guiButton);

			if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
				event.markAsUsed();
		}

		// Send DragEnd event to whichever element is active
		bool acceptEndDrag = mDragState == DragState::Dragging && mActiveMouseButton == guiButton && 
			mActiveElement != nullptr && (guiButton == GUIMouseButton::Left);

		if(acceptEndDrag)
		{
			mMouseEvent.setMouseDragEndData(mElementUnderCursor, localPos);
			if(sendMouseEvent(mActiveWidget, mActiveElement, mMouseEvent))
				event.markAsUsed();

			mDragState = DragState::NoDrag;
		}

		if(mActiveMouseButton == guiButton)
		{
			mActiveElement = nullptr;
			mActiveWidget = nullptr;
			mActiveMouseButton = GUIMouseButton::Left;
		}
	}

	void GUIManager::onCursorPressed(const CM::PositionalInputEvent& event)
	{
		if(event.isUsed())
			return;

		bool buttonStates[(int)GUIMouseButton::Count];
		buttonStates[0] = event.buttonStates[0];
		buttonStates[1] = event.buttonStates[1];
		buttonStates[2] = event.buttonStates[2];

		if(findElementUnderCursor(event.screenPos, buttonStates, event.shift, event.control, event.alt))
			event.markAsUsed();

		mMouseEvent = GUIMouseEvent(buttonStates, event.shift, event.control, event.alt);

		GUIMouseButton guiButton = buttonToGUIButton(event.button);

		// Send out selective input callback when user clicks on non-selectable element
		if(mSelectiveInputActive && mElementUnderCursor == nullptr)
		{
			if(mOnOutsideClickCallback != nullptr)
				mOnOutsideClickCallback();
		}

		// We only check for mouse down if mouse isn't already being held down, and we are hovering over an element
		bool acceptMouseDown = mActiveElement == nullptr && mElementUnderCursor != nullptr;
		if(acceptMouseDown)
		{
			Int2 localPos = getWidgetRelativePos(*mWidgetUnderCursor, event.screenPos);

			mMouseEvent.setMouseDownData(mElementUnderCursor, localPos, guiButton);
			if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
				event.markAsUsed();

			if(guiButton == GUIMouseButton::Left)
			{
				mDragState = DragState::HeldWithoutDrag;
				mLastCursorClickPos = event.screenPos;
			}

			mActiveElement = mElementUnderCursor;
			mActiveWidget = mWidgetUnderCursor;
			mActiveMouseButton = guiButton;
		}

		if(mElementUnderCursor != nullptr && mElementUnderCursor->_acceptsKeyboardFocus())
		{
			if(mKeyboardFocusElement != nullptr && mElementUnderCursor != mKeyboardFocusElement)
				mKeyboardFocusElement->_setFocus(false);

			mElementUnderCursor->_setFocus(true);

			mKeyboardFocusElement = mElementUnderCursor;
			mKeyboardFocusWidget = mWidgetUnderCursor;

			event.markAsUsed();
		}

		// If right click try to open context menu
		if(mElementUnderCursor != nullptr && buttonStates[2] == true) 
		{
			GUIContextMenu* menu = mElementUnderCursor->getContextMenu();

			if(menu != nullptr)
			{
				const RenderWindow* window = getWidgetWindow(*mWidgetUnderCursor);
				Int2 windowPos = window->screenToWindowPos(event.screenPos);

				menu->open(windowPos, *mWidgetUnderCursor);
				event.markAsUsed();
			}
		}
	}

	void GUIManager::onCursorDoubleClick(const CM::PositionalInputEvent& event)
	{
		if(event.isUsed())
			return;

		bool buttonStates[(int)GUIMouseButton::Count];
		buttonStates[0] = event.buttonStates[0];
		buttonStates[1] = event.buttonStates[1];
		buttonStates[2] = event.buttonStates[2];

		if(findElementUnderCursor(event.screenPos, buttonStates, event.shift, event.control, event.alt))
			event.markAsUsed();

		mMouseEvent = GUIMouseEvent(buttonStates, event.shift, event.control, event.alt);

		GUIMouseButton guiButton = buttonToGUIButton(event.button);

		// We only check for mouse down if we are hovering over an element
		bool acceptMouseDown = mElementUnderCursor != nullptr;
		if(acceptMouseDown)
		{
			Int2 localPos = getWidgetRelativePos(*mWidgetUnderCursor, event.screenPos);

			mMouseEvent.setMouseDoubleClickData(mElementUnderCursor, localPos, guiButton);
			if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
				event.markAsUsed();
		}
	}

	void GUIManager::onInputCommandEntered(CM::InputCommandType commandType)
	{
		if(mKeyboardFocusElement == nullptr)
			return;

		mCommandEvent = GUICommandEvent();

		switch(commandType)
		{
		case InputCommandType::Backspace:
			mCommandEvent.setType(GUICommandEventType::Backspace);
			break;
		case InputCommandType::Delete:
			mCommandEvent.setType(GUICommandEventType::Delete);
			break;
		case InputCommandType::Copy:
			mCommandEvent.setType(GUICommandEventType::Copy);
			break;
		case InputCommandType::Cut:
			mCommandEvent.setType(GUICommandEventType::Cut);
			break;
		case InputCommandType::Paste:
			mCommandEvent.setType(GUICommandEventType::Paste);
			break;
		case InputCommandType::Undo:
			mCommandEvent.setType(GUICommandEventType::Undo);
			break;
		case InputCommandType::Redo:
			mCommandEvent.setType(GUICommandEventType::Redo);
			break;
		case InputCommandType::Return:
			mCommandEvent.setType(GUICommandEventType::Return);
			break;
		case InputCommandType::Escape:
			mCommandEvent.setType(GUICommandEventType::Escape);
			break;
		case InputCommandType::Tab:
			mCommandEvent.setType(GUICommandEventType::Tab);
			break;
		case InputCommandType::SelectAll:
			mCommandEvent.setType(GUICommandEventType::SelectAll);
			break;
		case InputCommandType::CursorMoveLeft:
			mCommandEvent.setType(GUICommandEventType::CursorMoveLeft);
			break;
		case InputCommandType::CursorMoveRight:
			mCommandEvent.setType(GUICommandEventType::CursorMoveRight);
			break;
		case InputCommandType::CursorMoveUp:
			mCommandEvent.setType(GUICommandEventType::CursorMoveUp);
			break;
		case InputCommandType::CursorMoveDown:
			mCommandEvent.setType(GUICommandEventType::CursorMoveDown);
			break;
		case InputCommandType::SelectLeft:
			mCommandEvent.setType(GUICommandEventType::SelectLeft);
			break;
		case InputCommandType::SelectRight:
			mCommandEvent.setType(GUICommandEventType::SelectRight);
			break;
		case InputCommandType::SelectUp:
			mCommandEvent.setType(GUICommandEventType::SelectUp);
			break;
		case InputCommandType::SelectDown:
			mCommandEvent.setType(GUICommandEventType::SelectDown);
			break;
		}

		sendCommandEvent(mKeyboardFocusWidget, mKeyboardFocusElement, mCommandEvent);
	}

	bool GUIManager::findElementUnderCursor(const CM::Int2& cursorScreenPos, bool buttonStates[3], bool shift, bool control, bool alt)
	{
		Vector<const RenderWindow*>::type widgetWindows;
		for(auto& widgetInfo : mWidgets)
			widgetWindows.push_back(getWidgetWindow(*widgetInfo.widget));

#if CM_DEBUG_MODE
		// Checks if all referenced windows actually exist
		Vector<RenderWindow*>::type activeWindows = RenderWindowManager::instance().getRenderWindows();
		for(auto& window : widgetWindows)
		{
			if(window == nullptr)
				continue;

			auto iterFind = std::find(begin(activeWindows), end(activeWindows), window);

			if(iterFind == activeWindows.end())
			{
				CM_EXCEPT(InternalErrorException, "GUI manager has a reference to a window that doesn't exist. \
												  Please detach all GUIWidgets from windows before destroying a window.");
			}
		}
#endif

		GUIWidget* cursorOverWidget = nullptr;
		GUIElement* cursorOverElement = nullptr;

		const RenderWindow* windowUnderCursor = nullptr;
		UnorderedSet<const RenderWindow*>::type uniqueWindows;

		for(auto& window : widgetWindows)
		{
			if(window == nullptr)
				continue;

			uniqueWindows.insert(window);
		}

		for(auto& window : uniqueWindows)
		{
			if(Platform::isPointOverWindow(*window, cursorScreenPos))
			{
				windowUnderCursor = window;
				break;
			}
		}

		if(windowUnderCursor != nullptr)
		{
			Int2 windowPos = windowUnderCursor->screenToWindowPos(cursorScreenPos);
			Vector4 vecWindowPos((float)windowPos.x, (float)windowPos.y, 0.0f, 1.0f);

			UINT32 topMostDepth = std::numeric_limits<UINT32>::max();
			UINT32 widgetIdx = 0;
			for(auto& widgetInfo : mWidgets)
			{
				if(widgetWindows[widgetIdx] == nullptr)
				{
					widgetIdx++;
					continue;
				}

				GUIWidget* widget = widgetInfo.widget;
				if(widgetWindows[widgetIdx] == windowUnderCursor && widget->inBounds(windowToBridgedCoords(*widget, windowPos)))
				{
					SelectiveInputData* selectiveInputData = nullptr;
					if(mSelectiveInputActive)
					{
						auto selectionIterFind = mSelectiveInputData.find(widget);

						if(selectionIterFind == mSelectiveInputData.end())
						{
							widgetIdx++;
							continue;
						}
						else
							selectiveInputData = &selectionIterFind->second;
					}

					Int2 localPos = getWidgetRelativePos(*widget, cursorScreenPos);

					Vector<GUIElement*>::type sortedElements = widget->getElements();
					std::sort(sortedElements.begin(), sortedElements.end(), 
						[](GUIElement* a, GUIElement* b)
					{
						return a->_getDepth() < b->_getDepth();
					});

					// Elements with lowest depth (most to the front) get handled first
					for(auto iter = sortedElements.begin(); iter != sortedElements.end(); ++iter)
					{
						GUIElement* element = *iter;

						if(!element->_isDisabled() && element->_isInBounds(localPos) && element->_getDepth() < topMostDepth)
						{
							if(mSelectiveInputActive && !selectiveInputData->acceptAllElements)
							{
								if(selectiveInputData->elements.find(element) == selectiveInputData->elements.end())
									continue;
							}

							cursorOverElement = element;
							topMostDepth = element->_getDepth();
							cursorOverWidget = widget;

							break;
						}
					}
				}

				widgetIdx++;
			}
		}

		return handleCursorOver(cursorOverWidget, cursorOverElement, cursorScreenPos, buttonStates, shift, control, alt);
	}

	bool GUIManager::handleCursorOver(GUIWidget* widget, GUIElement* element, const CM::Int2& screenPos,
		bool buttonStates[3], bool shift, bool control, bool alt)
	{
		bool eventProcessed = false;

		Int2 localPos;
		if(widget != nullptr)
			localPos = getWidgetRelativePos(*widget, screenPos);

		mMouseEvent = GUIMouseEvent(buttonStates, shift, control, alt);

		// Send MouseOver/MouseOut events to any elements the mouse passes over, except when
		// mouse is being held down, in which we only send them to the active element
		if(element != mElementUnderCursor)
		{
			if(mElementUnderCursor != nullptr)
			{
				// Send MouseOut event
				if(mActiveElement == nullptr || mElementUnderCursor == mActiveElement)
				{
					Int2 curLocalPos = getWidgetRelativePos(*mWidgetUnderCursor, screenPos);

					mMouseEvent.setMouseOutData(element, curLocalPos);
					if(sendMouseEvent(mWidgetUnderCursor, mElementUnderCursor, mMouseEvent))
						eventProcessed = true;
				}
			}

			if(element != nullptr)
			{
				// Send MouseOver event
				if(mActiveElement == nullptr || element == mActiveElement)
				{
					mMouseEvent.setMouseOverData(element, localPos);
					if(sendMouseEvent(widget, element, mMouseEvent))
						eventProcessed = true;
				}
			}
		}

		mElementUnderCursor = element;
		mWidgetUnderCursor = widget;

		return eventProcessed;
	}

	void GUIManager::onTextInput(const CM::TextInputEvent& event)
	{
		if(mKeyboardFocusElement != nullptr)
		{
			mTextInputEvent = GUITextInputEvent();

			mTextInputEvent.setData(event.textChar);
			if(sendTextInputEvent(mKeyboardFocusWidget, mKeyboardFocusElement, mTextInputEvent))
				event.markAsUsed();
		}
	}

	void GUIManager::onWindowFocusGained(RenderWindow& win)
	{
		for(auto& widgetInfo : mWidgets)
		{
			GUIWidget* widget = widgetInfo.widget;
			if(getWidgetWindow(*widget) == &win)
				widget->ownerWindowFocusChanged();
		}
	}

	void GUIManager::onWindowFocusLost(RenderWindow& win)
	{
		for(auto& widgetInfo : mWidgets)
		{
			GUIWidget* widget = widgetInfo.widget;
			if(getWidgetWindow(*widget) == &win)
				widget->ownerWindowFocusChanged();
		}
	}

	// We stop getting mouse move events once it leaves the window, so make sure
	// nothing stays in hover state
	void GUIManager::onMouseLeftWindow(CM::RenderWindow* win)
	{
		bool buttonStates[3];
		buttonStates[0] = false;
		buttonStates[1] = false;
		buttonStates[2] = false;

		handleCursorOver(nullptr, nullptr, Int2(), buttonStates, false, false, false);
	}

	void GUIManager::queueForDestroy(GUIElement* element)
	{
		mScheduledForDestruction.push(element);
	}

	void GUIManager::processDestroyQueue()
	{
		while(!mScheduledForDestruction.empty())
		{
			cm_delete<PoolAlloc>(mScheduledForDestruction.top());
			mScheduledForDestruction.pop();
		}
	}

	// HACK - This callback is very hackish and very specific. Attempt to replace it with a more
	// general purpose solution
	void GUIManager::enableSelectiveInput(std::function<void()> onOutsideClickCallback)
	{
		mSelectiveInputActive = true;
		mOnOutsideClickCallback = onOutsideClickCallback;
	}

	void GUIManager::disableSelectiveInput()
	{
		mSelectiveInputData.clear();
		mSelectiveInputActive = false;
		mOnOutsideClickCallback = nullptr;
	}

	void GUIManager::addSelectiveInputWidget(const GUIWidget* widget)
	{
		auto findIter = mSelectiveInputData.find(widget);

		if(findIter == mSelectiveInputData.end())
		{
			SelectiveInputData& data = mSelectiveInputData[widget];
			data.acceptAllElements = true;
		}
		else
		{
			mSelectiveInputData[widget].acceptAllElements = true;
		}
	}

	void GUIManager::addSelectiveInputElement(const GUIElement* element)
	{
		mSelectiveInputData[&element->_getParentWidget()].elements.insert(element);
	}

	void GUIManager::setInputBridge(const CM::RenderTexture* renderTex, const GUIElement* element)
	{
		if(element == nullptr)
			mInputBridge.erase(renderTex);
		else
			mInputBridge[renderTex] = element;
	}

	GUIMouseButton GUIManager::buttonToGUIButton(PositionalInputEventButton cursorButton) const
	{
		if(cursorButton == PositionalInputEventButton::Left)
			return GUIMouseButton::Left;
		else if(cursorButton == PositionalInputEventButton::Middle)
			return GUIMouseButton::Middle;
		else if(cursorButton == PositionalInputEventButton::Right)
			return GUIMouseButton::Right;

		CM_EXCEPT(InvalidParametersException, "Provided button is not a GUI supported mouse button.");
	}

	Int2 GUIManager::getWidgetRelativePos(const GUIWidget& widget, const Int2& screenPos) const
	{
		const RenderWindow* window = getWidgetWindow(widget);
		Int2 windowPos = window->screenToWindowPos(screenPos);
		windowPos = windowToBridgedCoords(widget, windowPos);

		const Matrix4& worldTfrm = widget.SO()->getWorldTfrm();

		Vector4 vecLocalPos = worldTfrm.inverse() * Vector4((float)windowPos.x, (float)windowPos.y, 0.0f, 1.0f);
		Int2 curLocalPos(Math::RoundToInt(vecLocalPos.x), Math::RoundToInt(vecLocalPos.y));

		return curLocalPos;
	}

	Int2 GUIManager::windowToBridgedCoords(const GUIWidget& widget, const Int2& windowPos) const
	{
		// This cast might not be valid (the render target could be a window), but we only really need to cast
		// so that mInputBridge map allows us to search through it - we don't access anything unless the target is bridged
		// (in which case we know it is a RenderTexture)
		const RenderTexture* renderTexture = static_cast<const RenderTexture*>(widget.getTarget()->getTarget().get());

		auto iterFind = mInputBridge.find(renderTexture);
		if(iterFind != mInputBridge.end()) // Widget input is bridged, which means we need to transform the coordinates
		{
			const GUIElement* bridgeElement = iterFind->second;

			const Matrix4& worldTfrm = bridgeElement->_getParentWidget().SO()->getWorldTfrm();

			Vector4 vecLocalPos = worldTfrm.inverse() * Vector4((float)windowPos.x, (float)windowPos.y, 0.0f, 1.0f);
			Rect bridgeBounds = bridgeElement->getBounds();

			// Find coordinates relative to the bridge element
			float x = vecLocalPos.x - (float)bridgeBounds.x;
			float y = vecLocalPos.y - (float)bridgeBounds.y;

			float scaleX = renderTexture->getWidth() / (float)bridgeBounds.width;
			float scaleY = renderTexture->getHeight() / (float)bridgeBounds.height;

			return Int2(Math::RoundToInt(x * scaleX), Math::RoundToInt(y * scaleY));
		}

		return windowPos;
	}

	const CM::RenderWindow* GUIManager::getWidgetWindow(const GUIWidget& widget) const
	{
		// This cast might not be valid (the render target could be a window), but we only really need to cast
		// so that mInputBridge map allows us to search through it - we don't access anything unless the target is bridged
		// (in which case we know it is a RenderTexture)
		const RenderTexture* renderTexture = static_cast<const RenderTexture*>(widget.getTarget()->getTarget().get());

		auto iterFind = mInputBridge.find(renderTexture);
		if(iterFind != mInputBridge.end())
		{
			GUIWidget& parentWidget = iterFind->second->_getParentWidget();
			if(&parentWidget != &widget)
			{
				return getWidgetWindow(parentWidget);
			}
		}

		RenderTargetPtr renderTarget = widget.getTarget()->getTarget();
		Vector<RenderWindow*>::type renderWindows = RenderWindowManager::instance().getRenderWindows();

		auto iterFindWin = std::find(renderWindows.begin(), renderWindows.end(), renderTarget.get());
		if(iterFindWin != renderWindows.end())
			return static_cast<RenderWindow*>(renderTarget.get());

		return nullptr;
	}

	bool GUIManager::sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event)
	{
		if(!mouseEventFilter.empty())
			mouseEventFilter(widget, element, event);

		return widget->_mouseEvent(element, event);
	}

	bool GUIManager::sendTextInputEvent(GUIWidget* widget, GUIElement* element, const GUITextInputEvent& event)
	{
		if(!textInputEventFilter.empty())
			textInputEventFilter(widget, element, event);

		return widget->_textInputEvent(element, event);
	}

	bool GUIManager::sendCommandEvent(GUIWidget* widget, GUIElement* element, const GUICommandEvent& event)
	{
		return widget->_commandEvent(element, event);
	}

	GUIManager& gGUIManager()
	{
		return GUIManager::instance();
	}
}