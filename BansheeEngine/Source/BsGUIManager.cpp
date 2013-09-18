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
#include "BsGUIDropDownList.h"
#include "BsGUIDropDownBox.h"
#include "BsDragAndDropManager.h"

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

	GUIManager::GUIManager()
		:mMouseOverElement(nullptr), mMouseOverWidget(nullptr), mSeparateMeshesByWidget(true), mActiveElement(nullptr), 
		mActiveWidget(nullptr), mActiveMouseButton(GUIMouseButton::Left), mKeyboardFocusElement(nullptr), mKeyboardFocusWidget(nullptr),
		mCaretTexture(nullptr), mCaretBlinkInterval(0.5f), mCaretLastBlinkTime(0.0f), mCaretColor(1.0f, 0.6588f, 0.0f), mIsCaretOn(false),
		mTextSelectionColor(1.0f, 0.6588f, 0.0f), mInputCaret(nullptr), mInputSelection(nullptr), mDropDownBoxActive(false), mDropDownBoxOpenScheduled(false)
	{
		mOnButtonDownConn = gInput().onButtonDown.connect(boost::bind(&GUIManager::onButtonDown, this, _1));
		mOnButtonUpConn = gInput().onButtonUp.connect(boost::bind(&GUIManager::onButtonUp, this, _1));
		mOnMouseMovedConn = gInput().onMouseMoved.connect(boost::bind(&GUIManager::onMouseMoved, this, _1));
		mOnTextInputConn = gInput().onCharInput.connect(boost::bind(&GUIManager::onTextInput, this, _1)); 

		mWindowGainedFocusConn = RenderWindowManager::instance().onFocusGained.connect(boost::bind(&GUIManager::onWindowFocusGained, this, _1));
		mWindowLostFocusConn = RenderWindowManager::instance().onFocusLost.connect(boost::bind(&GUIManager::onWindowFocusLost, this, _1));
		mWindowMovedOrResizedConn = RenderWindowManager::instance().onMovedOrResized.connect(boost::bind(&GUIManager::onWindowMovedOrResized, this, _1));

		mMouseLeftWindowConn = Platform::onMouseLeftWindow.connect(boost::bind(&GUIManager::onMouseLeftWindow, this, _1));

		mInputCaret = cm_new<GUIInputCaret, PoolAlloc>();
		mInputSelection = cm_new<GUIInputSelection, PoolAlloc>();

		DragAndDropManager::startUp(cm_new<DragAndDropManager>());
		mMouseDragEndedConn = DragAndDropManager::instance().onDragEnded.connect(boost::bind(&GUIManager::onMouseDragEnded, this, _1));

		// Need to defer this call because I want to make sure all managers are initialized first
		deferredCall(std::bind(&GUIManager::updateCaretTexture, this));
		deferredCall(std::bind(&GUIManager::updateTextSelectionTexture, this));
	}

	GUIManager::~GUIManager()
	{
		DragAndDropManager::shutDown();

		// Make a copy of widgets, since destroying them will remove them from mWidgets and
		// we can't iterate over an array thats getting modified
		Vector<WidgetInfo>::type widgetCopy = mWidgets;
		for(auto& widget : widgetCopy)
			widget.widget->destroy();

		mOnButtonDownConn.disconnect();
		mOnButtonUpConn.disconnect();
		mOnMouseMovedConn.disconnect();
		mOnTextInputConn.disconnect();

		mMouseDragEndedConn.disconnect();

		mWindowGainedFocusConn.disconnect();
		mWindowLostFocusConn.disconnect();
		mWindowMovedOrResizedConn.disconnect();

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

		if(mMouseOverWidget == widget)
		{
			mMouseOverWidget = nullptr;
			mMouseOverElement = nullptr;
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

		// We only activate the drop down box the next frame so it isn't
		// accidentally destroyed the same frame it was created
		if(mDropDownBoxOpenScheduled)
		{
			mDropDownBoxActive = true;
			mDropDownBoxOpenScheduled = false;
		}

		// Update layouts
		for(auto& widgetInfo : mWidgets)
		{
			widgetInfo.widget->_updateLayout();
		}

		// Blink caret
		if(mKeyboardFocusElement != nullptr)
		{
			float curTime = gTime().getTime();

			if((curTime - mCaretLastBlinkTime) >= mCaretBlinkInterval)
			{
				mCaretLastBlinkTime = curTime;
				mIsCaretOn = !mIsCaretOn;

				mCommandEvent = GUICommandEvent();
				mCommandEvent.setRedrawData();
				mKeyboardFocusElement->commandEvent(mCommandEvent);
			}
		}

		updateMeshes();

		if(mMouseOverElement != nullptr && mMouseOverElement->_isDestroyed())
		{
			mMouseOverElement = nullptr;
			mMouseOverWidget = nullptr;
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
					continue;

				if(mesh == nullptr || !mesh.isLoaded())
					continue;

				material->setFloat("invViewportWidth", invViewportWidth);
				material->setFloat("invViewportHeight", invViewportHeight);
				material->setMat4("worldTransform", widget->SO()->getWorldTfrm());

				renderQueue.add(material, mesh->getSubMeshData(), Vector3::ZERO);

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

				gMainSyncedCA().writeSubresource(renderData.cachedMeshes[groupIdx].getInternalPtr(), 0, *meshData);
				gMainSyncedCA().submitToCoreThread(true); // TODO - Remove this once I make writeSubresource accept a shared_ptr for MeshData

				groupIdx++;
			}
		}
	}

	void GUIManager::openDropDownBox(GUIDropDownList* parentList, const CM::Vector<WString>::type& elements, 
		std::function<void(CM::UINT32)> selectedCallback)
	{
		if(mDropDownBoxOpenScheduled || mDropDownBoxActive)
			closeDropDownBox(-1);

		mDropDownSO = SceneObject::create("DropDownBox");
		mDropDownBox = mDropDownSO->addComponent<GUIDropDownBox>();

		GUIWidget& widget = parentList->_getParentWidget();
		mDropDownBox->initialize(widget.getTarget(), widget.getOwnerWindow(), parentList, elements, 
			boost::bind(&GUIManager::closeDropDownBox, this, _1));

		mDropDownBoxOpenScheduled = true;
		mDropDownSelectionMade = selectedCallback;
	}

	void GUIManager::closeDropDownBox(INT32 selectedIdx)
	{
		if(selectedIdx != -1)
			mDropDownSelectionMade(selectedIdx);

		mDropDownBoxActive = false;

		mDropDownSO->destroy();
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

		gMainSyncedCA().writeSubresource(tex.getInternalPtr(), tex->mapToSubresourceIdx(0, 0), *data);
		gMainSyncedCA().submitToCoreThread(true); // TODO - Remove this once I make writeSubresource accept a shared_ptr for MeshData
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

		gMainSyncedCA().writeSubresource(tex.getInternalPtr(), tex->mapToSubresourceIdx(0, 0), *data);
		gMainSyncedCA().submitToCoreThread(true); // TODO - Remove this once I make writeSubresource accept a shared_ptr for MeshData
	}

	void GUIManager::onButtonDown(const ButtonEvent& event)
	{
		if(event.isUsed())
			return;

		bool shiftDown = gInput().isButtonDown(BC_LSHIFT) || gInput().isButtonDown(BC_RSHIFT);
		bool ctrlDown = gInput().isButtonDown(BC_LCONTROL) || gInput().isButtonDown(BC_RCONTROL);
		bool altDown = gInput().isButtonDown(BC_LMENU) || gInput().isButtonDown(BC_RMENU);

		if(event.isKeyboard())
		{
			if(mKeyboardFocusElement != nullptr)
			{
				mKeyEvent = GUIKeyEvent(shiftDown, ctrlDown, altDown);

				mKeyEvent.setKeyDownData(event.buttonCode);
				if(sendKeyEvent(mKeyboardFocusWidget, mKeyboardFocusElement, mKeyEvent))
					event.markAsUsed();
			}
		}

		if(event.isMouse())
		{
			bool buttonStates[(int)GUIMouseButton::Count];
			buttonStates[0] = gInput().isButtonDown(BC_MOUSE_LEFT);
			buttonStates[1] = gInput().isButtonDown(BC_MOUSE_MIDDLE);
			buttonStates[2] = gInput().isButtonDown(BC_MOUSE_RIGHT);

			mMouseEvent = GUIMouseEvent(buttonStates, shiftDown, ctrlDown, altDown);

			GUIMouseButton guiButton = buttonToMouseButton(event.buttonCode);

			// We only check for mouse down if mouse isn't already being held down, and we are hovering over an element
			bool acceptMouseDown = mActiveElement == nullptr && mMouseOverElement != nullptr;
			if(acceptMouseDown)
			{
				Int2 localPos = getWidgetRelativePos(*mMouseOverWidget, gInput().getMousePosition());

				mMouseEvent.setMouseDownData(mMouseOverElement, localPos, guiButton);
				if(sendMouseEvent(mMouseOverWidget, mMouseOverElement, mMouseEvent))
					event.markAsUsed();

				// DragStart is for all intents and purposes same as mouse down but since I need a DragEnd event, I feel a separate DragStart
				// event was also needed to make things clearer.
				mMouseEvent.setMouseDragStartData(mMouseOverElement, localPos);
				if(sendMouseEvent(mMouseOverWidget, mMouseOverElement, mMouseEvent))
					event.markAsUsed();

				mActiveElement = mMouseOverElement;
				mActiveWidget = mMouseOverWidget;
				mActiveMouseButton = guiButton;
			}

			if(mMouseOverElement != nullptr && mMouseOverElement->_acceptsKeyboardFocus())
			{
				if(mKeyboardFocusElement != nullptr && mMouseOverElement != mKeyboardFocusElement)
					mKeyboardFocusElement->_setFocus(false);

				mMouseOverElement->_setFocus(true);

				mKeyboardFocusElement = mMouseOverElement;
				mKeyboardFocusWidget = mMouseOverWidget;

				event.markAsUsed();
			}

			// Close drop down box if user clicks outside the drop down box
			if(mDropDownBoxActive)
			{
				if(mMouseOverElement == nullptr || (&mMouseOverElement->_getParentWidget() != mDropDownBox.get()))
				{
					closeDropDownBox(-1);
				}
			}
		}
	}

	void GUIManager::onButtonUp(const ButtonEvent& event)
	{
		if(event.isUsed())
			return;

		bool shiftDown = gInput().isButtonDown(BC_LSHIFT) || gInput().isButtonDown(BC_RSHIFT);
		bool ctrlDown = gInput().isButtonDown(BC_LCONTROL) || gInput().isButtonDown(BC_RCONTROL);
		bool altDown = gInput().isButtonDown(BC_LMENU) || gInput().isButtonDown(BC_RMENU);

		if(event.isKeyboard())
		{
			if(mKeyboardFocusElement != nullptr)
			{
				mKeyEvent = GUIKeyEvent(shiftDown, ctrlDown, altDown);

				mKeyEvent.setKeyUpData(event.buttonCode);
				if(sendKeyEvent(mKeyboardFocusWidget, mKeyboardFocusElement, mKeyEvent))
					event.markAsUsed();
			}
		}

		if(event.isMouse())
		{
			bool buttonStates[(int)GUIMouseButton::Count];
			buttonStates[0] = gInput().isButtonDown(BC_MOUSE_LEFT);
			buttonStates[1] = gInput().isButtonDown(BC_MOUSE_MIDDLE);
			buttonStates[2] = gInput().isButtonDown(BC_MOUSE_RIGHT);

			mMouseEvent = GUIMouseEvent(buttonStates, shiftDown, ctrlDown, altDown);

			Int2 localPos;
			if(mMouseOverWidget != nullptr)
			{
				localPos = getWidgetRelativePos(*mMouseOverWidget, gInput().getMousePosition());
			}

			GUIMouseButton guiButton = buttonToMouseButton(event.buttonCode);

			// Send MouseUp event only if we are over the active element (we don't want to accidentally trigger other elements).
			// And only activate when a button that originally caused the active state is released, otherwise ignore it.
			bool acceptMouseUp = mActiveMouseButton == guiButton && (mMouseOverElement != nullptr && mActiveElement == mMouseOverElement);
			if(acceptMouseUp)
			{
				mMouseEvent.setMouseUpData(mMouseOverElement, localPos, guiButton);
				
				if(sendMouseEvent(mMouseOverWidget, mMouseOverElement, mMouseEvent))
					event.markAsUsed();
			}

			// Send DragEnd event to whichever element is active
			bool acceptEndDrag = mActiveMouseButton == guiButton && mActiveElement != nullptr;
			if(acceptEndDrag)
			{
				mMouseEvent.setMouseDragEndData(mMouseOverElement, localPos);
				if(sendMouseEvent(mActiveWidget, mActiveElement, mMouseEvent))
					event.markAsUsed();

				mActiveElement = nullptr;
				mActiveWidget = nullptr;
				mActiveMouseButton = GUIMouseButton::Left;
			}
		}
	}

	bool GUIManager::onMouseDragEnded(const CM::ButtonEvent& event)
	{
		GUIMouseButton guiButton = buttonToMouseButton(event.buttonCode);

		if(DragAndDropManager::instance().isDragInProgress() && guiButton == GUIMouseButton::Left)
		{
			if(mMouseOverElement != nullptr)
			{
				mMouseEvent.setDragAndDropDroppedData(mMouseOverElement, Int2(), DragAndDropManager::instance().getDragTypeId(), DragAndDropManager::instance().getDragData());
				bool processed = sendMouseEvent(mMouseOverWidget, mMouseOverElement, mMouseEvent);

				return processed;
			}
		}

		return false;
	}

	void GUIManager::onMouseMoved(const MouseEvent& event)
	{
		if(event.isUsed())
			return;

#if CM_DEBUG_MODE
		// Checks if all referenced windows actually exist
		Vector<RenderWindow*>::type activeWindows = RenderWindowManager::instance().getRenderWindows();
		for(auto& widgetInfo : mWidgets)
		{
			auto iterFind = std::find(begin(activeWindows), end(activeWindows), widgetInfo.widget->getOwnerWindow());

			if(iterFind == activeWindows.end())
			{
				CM_EXCEPT(InternalErrorException, "GUI manager has a reference to a window that doesn't exist. \
												  Please detach all GUIWidgets from windows before destroying a window.");
			}
		}
#endif

		GUIWidget* widgetInFocus = nullptr;
		GUIElement* topMostElement = nullptr;
		Int2 screenPos;
		Int2 localPos;

		for(auto& widgetInfo : mWidgets)
		{
			const RenderWindow* window = widgetInfo.widget->getOwnerWindow();

			if(window->hasFocus())
			{
				widgetInFocus = widgetInfo.widget;
				break;
			}
		}

		if(widgetInFocus != nullptr)
		{
			const RenderWindow* window = widgetInFocus->getOwnerWindow();

			Int2 screenPos = window->screenToWindowPos(event.screenPos);
			Vector4 vecScreenPos((float)screenPos.x, (float)screenPos.y, 0.0f, 1.0f);

			UINT32 topMostDepth = std::numeric_limits<UINT32>::max();
			for(auto& widgetInfo : mWidgets)
			{
				GUIWidget* widget = widgetInfo.widget;
				if(widget->getOwnerWindow() == window && widget->inBounds(screenPos))
				{
					const Matrix4& worldTfrm = widget->SO()->getWorldTfrm();

					Vector4 vecLocalPos = worldTfrm.inverse() * vecScreenPos;
					localPos = Int2(Math::RoundToInt(vecLocalPos.x), Math::RoundToInt(vecLocalPos.y));

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
							topMostElement = element;
							topMostDepth = element->_getDepth();
							widgetInFocus = widget;

							break;
						}
					}
				}
			}
		}

		if(handleMouseOver(widgetInFocus, topMostElement, event.screenPos, event.mouseWheelScrollAmount))
			event.markAsUsed();
	}

	void GUIManager::onTextInput(const CM::TextInputEvent& event)
	{
		if(mKeyboardFocusElement != nullptr)
		{
			bool shiftDown = gInput().isButtonDown(BC_LSHIFT) || gInput().isButtonDown(BC_RSHIFT);
			bool ctrlDown = gInput().isButtonDown(BC_LCONTROL) || gInput().isButtonDown(BC_RCONTROL);
			bool altDown = gInput().isButtonDown(BC_LMENU) || gInput().isButtonDown(BC_RMENU);

			if(ctrlDown || altDown) // Ignore text input because key characters + alt/ctrl usually correspond to certain commands
				return;

			mKeyEvent = GUIKeyEvent(shiftDown, ctrlDown, altDown);

			mKeyEvent.setTextInputData(event.textChar);
			if(sendKeyEvent(mKeyboardFocusWidget, mKeyboardFocusElement, mKeyEvent))
				event.markAsUsed();
		}
	}

	bool GUIManager::handleMouseOver(GUIWidget* widget, GUIElement* element, const CM::Int2& screenMousePos, float wheelScrollAmount)
	{
		bool eventProcessed = false;

		Int2 localPos;
		if(widget != nullptr)
		{
			const RenderWindow* window = widget->getOwnerWindow();

			Int2 screenPos = window->screenToWindowPos(screenMousePos);
			Vector4 vecScreenPos((float)screenPos.x, (float)screenPos.y, 0.0f, 1.0f);

			const Matrix4& worldTfrm = widget->SO()->getWorldTfrm();

			Vector4 vecLocalPos = worldTfrm.inverse() * vecScreenPos;
			localPos = Int2(Math::RoundToInt(vecLocalPos.x), Math::RoundToInt(vecLocalPos.y));
		}

		bool shiftDown = gInput().isButtonDown(BC_LSHIFT) || gInput().isButtonDown(BC_RSHIFT);
		bool ctrlDown = gInput().isButtonDown(BC_LCONTROL) || gInput().isButtonDown(BC_RCONTROL);
		bool altDown = gInput().isButtonDown(BC_LMENU) || gInput().isButtonDown(BC_RMENU);

		// TODO - Maybe avoid querying these for every event separately?
		bool buttonStates[(int)GUIMouseButton::Count];
		buttonStates[0] = gInput().isButtonDown(BC_MOUSE_LEFT);
		buttonStates[1] = gInput().isButtonDown(BC_MOUSE_MIDDLE);
		buttonStates[2] = gInput().isButtonDown(BC_MOUSE_RIGHT);

		mMouseEvent = GUIMouseEvent(buttonStates, shiftDown, ctrlDown, altDown);

		// Send MouseOver/MouseOut events to any elements the mouse passes over, except when
		// mouse is being held down, in which we only send them to the active element
		if(element != mMouseOverElement)
		{
			if(mMouseOverElement != nullptr)
			{
				// Send MouseOut event
				if(mActiveElement == nullptr || mMouseOverElement == mActiveElement)
				{
					Int2 curLocalPos = getWidgetRelativePos(*mMouseOverWidget, screenMousePos);

					mMouseEvent.setMouseOutData(element, curLocalPos);
					if(sendMouseEvent(mMouseOverWidget, mMouseOverElement, mMouseEvent))
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

		// If mouse is being held down send MouseDrag events
		if(mActiveElement != nullptr)
		{
			Int2 curLocalPos = getWidgetRelativePos(*mActiveWidget, screenMousePos);

			if(mLastCursorLocalPos != curLocalPos)
			{
				mMouseEvent.setMouseDragData(element, curLocalPos, curLocalPos - mLastCursorLocalPos);
				if(sendMouseEvent(mActiveWidget, mActiveElement, mMouseEvent))
					eventProcessed = true;

				mLastCursorLocalPos = curLocalPos;
			}

			// Also if drag is in progress send DragAndDrop events
			if(DragAndDropManager::instance().isDragInProgress())
			{
				if(element != nullptr)
				{
					mMouseEvent.setDragAndDropDraggedData(element, localPos, DragAndDropManager::instance().getDragTypeId(), DragAndDropManager::instance().getDragData());
					if(sendMouseEvent(widget, element, mMouseEvent))
						eventProcessed = true;
				}
			}
		}
		else // Otherwise, send MouseMove events if we are hovering over any element
		{
			if(element != nullptr)
			{
				// Send MouseMove event
				if(mLastCursorLocalPos != localPos)
				{
					mMouseEvent.setMouseMoveData(element, localPos);
					if(sendMouseEvent(widget, element, mMouseEvent))
						eventProcessed = true;

					mLastCursorLocalPos = localPos;
				}

				if(Math::Abs(wheelScrollAmount) > 0.00001f)
				{
					mMouseEvent.setMouseWheelScrollData(element, wheelScrollAmount);
					if(sendMouseEvent(widget, element, mMouseEvent))
						eventProcessed = true;
				}
			}
		}

		mMouseOverElement = element;
		mMouseOverWidget = widget;

		return false;
	}

	void GUIManager::onWindowFocusGained(RenderWindow& win)
	{
		for(auto& widgetInfo : mWidgets)
		{
			GUIWidget* widget = widgetInfo.widget;
			if(widget->getOwnerWindow() == &win)
				widget->ownerWindowFocusChanged();
		}
	}

	void GUIManager::onWindowFocusLost(RenderWindow& win)
	{
		for(auto& widgetInfo : mWidgets)
		{
			GUIWidget* widget = widgetInfo.widget;
			if(widget->getOwnerWindow() == &win)
				widget->ownerWindowFocusChanged();
		}
	}

	void GUIManager::onWindowMovedOrResized(RenderWindow& win)
	{
		for(auto& widgetInfo : mWidgets)
		{
			GUIWidget* widget = widgetInfo.widget;
			if(widget->getOwnerWindow() == &win)
				widget->ownerWindowResized();
		}
	}

	// We stop getting mouse move events once it leaves the window, so make sure
	// nothing stays in hover state
	void GUIManager::onMouseLeftWindow(CM::RenderWindow* win)
	{
		handleMouseOver(nullptr, nullptr, Int2());
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

	GUIMouseButton GUIManager::buttonToMouseButton(ButtonCode code) const
	{
		if(code == BC_MOUSE_LEFT)
			return GUIMouseButton::Left;
		else if(code == BC_MOUSE_MIDDLE)
			return GUIMouseButton::Middle;
		else if(code == BC_MOUSE_RIGHT)
			return GUIMouseButton::Right;

		CM_EXCEPT(InvalidParametersException, "Provided button code is not a GUI supported mouse button.");
	}

	Int2 GUIManager::getWidgetRelativePos(const GUIWidget& widget, const Int2& screenPos) const
	{
		const RenderWindow* window = widget.getOwnerWindow();
		Int2 windowPos = window->screenToWindowPos(screenPos);

		const Matrix4& worldTfrm = widget.SO()->getWorldTfrm();

		Vector4 vecLocalPos = worldTfrm.inverse() * Vector4((float)windowPos.x, (float)windowPos.y, 0.0f, 1.0f);
		Int2 curLocalPos(Math::RoundToInt(vecLocalPos.x), Math::RoundToInt(vecLocalPos.y));

		return curLocalPos;
	}

	bool GUIManager::sendMouseEvent(GUIWidget* widget, GUIElement* element, const GUIMouseEvent& event)
	{
		if(!mouseEventFilter.empty())
			mouseEventFilter(widget, element, event);

		return widget->_mouseEvent(element, event);
	}

	bool GUIManager::sendKeyEvent(GUIWidget* widget, GUIElement* element, const GUIKeyEvent& event)
	{
		if(!keyEventFilter.empty())
			keyEventFilter(widget, element, event);

		return widget->_keyEvent(element, event);
	}

	GUIManager& gGUIManager()
	{
		return GUIManager::instance();
	}
}