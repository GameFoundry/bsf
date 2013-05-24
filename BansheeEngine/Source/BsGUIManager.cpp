#include "BsGUIManager.h"
#include "BsGUIWidget.h"
#include "BsGUIElement.h"
#include "BsGUIMouseEvent.h"
#include "CmSceneObject.h"
#include "CmMaterial.h"
#include "CmMeshData.h"
#include "CmMesh.h"
#include "CmUtil.h"
#include "CmRenderWindowManager.h"
#include "CmCursor.h"
#include "CmRect.h"
#include "CmApplication.h"
#include "CmException.h"
#include "CmInput.h"
#include "CmPass.h"

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
		std::vector<GUIGroupElement> elements;
	};

	GUIManager::GUIManager()
		:mMouseOverElement(nullptr), mMouseOverWidget(nullptr), mSeparateMeshesByWidget(true)
	{
		for(int i = 0; i < MB_Count; i++)
			mLastFrameButtonState[i] = false;
	}

	GUIManager::~GUIManager()
	{

	}

	void GUIManager::registerWidget(GUIWidget* widget)
	{
		mWidgets.push_back(widget);

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
		auto findIter = std::find(begin(mWidgets), end(mWidgets), widget);

		if(findIter != end(mWidgets))
			mWidgets.erase(findIter);

		if(mMouseOverWidget == widget)
		{
			mMouseOverWidget = nullptr;
			mMouseOverElement = nullptr;
		}

		const Viewport* renderTarget = widget->getTarget();
		GUIRenderData& renderData = mCachedGUIData[renderTarget];

		findIter = std::find(begin(renderData.widgets), end(renderData.widgets), widget);
		if(findIter != end(renderData.widgets))
			renderData.widgets.erase(findIter);

		if(renderData.widgets.size() == 0)
			mCachedGUIData.erase(renderTarget);
		else
			renderData.isDirty = true;
	}

	void GUIManager::update()
	{
		// Update layouts
		for(auto& widget : mWidgets)
		{
			widget->_updateLayout();
		}

		updateMeshes();
		updateInput();
	}

	void GUIManager::render(ViewportPtr& target, RenderContext& renderContext)
	{
		auto findIter = mCachedGUIData.find(target.get());

		if(findIter == mCachedGUIData.end())
			return;

		renderContext.setViewport(target);

		GUIRenderData& renderData = findIter->second;

		// Render the meshes
		if(mSeparateMeshesByWidget)
		{
			UINT32 meshIdx = 0;
			for(auto& mesh : renderData.cachedMeshes)
			{
				HMaterial material = renderData.cachedMaterials[meshIdx];
				GUIWidget* widget = renderData.cachedWidgetsPerMesh[meshIdx];

				// TODO - Possible optimization. I currently divide by width/height inside the shader, while it
				// might be more optimal to just scale the mesh as the resolution changes?
				float invViewportWidth = 1.0f / (target->getWidth() * 0.5f);
				float invViewportHeight = 1.0f / (target->getHeight() * 0.5f);

				material->setFloat("invViewportWidth", invViewportWidth);
				material->setFloat("invViewportHeight", invViewportHeight);
				material->setMat4("worldTransform", widget->SO()->getWorldTfrm());

				if(material == nullptr || !material.isLoaded())
					continue;

				if(mesh == nullptr || !mesh.isLoaded())
					continue;

				for(UINT32 i = 0; i < material->getNumPasses(); i++)
				{
					PassPtr pass = material->getPass(i);
					pass->activate(renderContext);

					PassParametersPtr paramsPtr = material->getPassParameters(i);
					pass->bindParameters(renderContext, paramsPtr);

					renderContext.render(mesh->getRenderOperation());
				}

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
				return aDepth > bDepth || (aDepth ==bDepth && a.element > b.element); 
			};

			std::set<GUIGroupElement, std::function<bool(const GUIGroupElement&, const GUIGroupElement&)>> allElements(elemComp);

			for(auto& widget : renderData.widgets)
			{
				const std::vector<GUIElement*>& elements = widget->getElements();

				for(auto& element : elements)
				{
					UINT32 numRenderElems = element->getNumRenderElements();
					for(UINT32 i = 0; i < numRenderElems; i++)
					{
						allElements.insert(GUIGroupElement(element, i));
					}
				}
			}

			// Group the elements in such a way so that we end up with a smallest amount of
			// meshes, without breaking back to front rendering order
			std::unordered_map<UINT64, std::vector<GUIMaterialGroup>> materialGroups;
			for(auto& elem : allElements)
			{
				GUIElement* guiElem = elem.element;
				UINT32 renderElemIdx = elem.renderElement;
				UINT32 elemDepth = guiElem->_getRenderElementDepth(renderElemIdx);

				Rect tfrmedBounds = guiElem->_getBounds();
				tfrmedBounds.transform(guiElem->_getParentWidget().SO()->getWorldTfrm());

				const HMaterial& mat = guiElem->getMaterial(renderElemIdx);

				UINT64 materialId = mat->getInternalID(); // TODO - I group based on material ID. So if two widgets used exact copies of the same material
				// this system won't detect it. Find a better way of determining material similarity?

				// If this is a new material, add a new list of groups
				auto findIterMaterial = materialGroups.find(materialId);
				if(findIterMaterial == end(materialGroups))
					materialGroups[materialId] = std::vector<GUIMaterialGroup>();

				// Try to find a group this material will fit in:
				//  - Group that has a depth value same or one below elements depth will always be a match
				//  - Otherwise, we search higher depth values as well, but we only use them if no elements in between those depth values
				//    overlap the current elements bounds.
				std::vector<GUIMaterialGroup>& allGroups = materialGroups[materialId];
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

			std::set<GUIMaterialGroup*, std::function<bool(GUIMaterialGroup*, GUIMaterialGroup*)>> sortedGroups(groupComp);
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

				MeshDataPtr meshData = std::shared_ptr<MeshData>(CM_NEW(MeshData, PoolAlloc) MeshData(group->numQuads * 4),
					&MemAllocDeleter<MeshData, PoolAlloc>::deleter);

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

				gMainSyncedRC().writeSubresource(renderData.cachedMeshes[groupIdx].getInternalPtr(), 0, *meshData);
				gMainSyncedRC().submitToGpu(true); // TODO - Remove this once I make writeSubresource accept a shared_ptr for MeshData

				groupIdx++;
			}
		}
	}

	void GUIManager::updateInput()
	{
#if CM_DEBUG_MODE
		// Checks if all referenced windows actually exist
		std::vector<RenderWindow*> activeWindows = RenderWindowManager::instance().getRenderWindows();
		for(auto& widget : mWidgets)
		{
			auto iterFind = std::find(begin(activeWindows), end(activeWindows), widget->getOwnerWindow());

			if(iterFind == activeWindows.end())
			{
				CM_EXCEPT(InternalErrorException, "GUI manager has a reference to a window that doesn't exist. \
												  Please detach all GUIWidgets from windows before destroying a window.");
			}
		}
#endif
		GUIWidget* widgetInFocus = nullptr;
		GUIElement* topMostElement = nullptr;
		UINT32 topMostDepth = std::numeric_limits<UINT32>::max();

		for(auto& widget : mWidgets)
		{
			const RenderWindow* window = widget->getOwnerWindow();

			if(window->hasFocus())
			{
				widgetInFocus = widget;
				break;
			}
		}

		Int2 screenPos;
		if(widgetInFocus != nullptr)
		{
			const RenderWindow* window = widgetInFocus->getOwnerWindow();

			screenPos = Cursor::getWindowPosition(*window);
			Vector4 vecScreenPos((float)screenPos.x, (float)screenPos.y, 0.0f, 1.0f);

			for(auto& widget : mWidgets)
			{
				if(widget->inBounds(screenPos))
				{
					const Matrix4& worldTfrm = widget->SO()->getWorldTfrm();

					Vector4 vecLocalPos = worldTfrm.inverse() * vecScreenPos;
					Int2 localPos(Math::RoundToInt(vecLocalPos.x), Math::RoundToInt(vecLocalPos.y));

					std::vector<GUIElement*> sortedElements = widget->getElements();
					std::sort(sortedElements.begin(), sortedElements.end(), 
						[](GUIElement* a, GUIElement* b)
					{
						return a->_getDepth() < b->_getDepth();
					});

					// Elements with lowest depth (most to the front) get handled first
					for(auto iter = sortedElements.begin(); iter != sortedElements.end(); ++iter)
					{
						GUIElement* element = *iter;
						const Rect& bounds = element->_getContentBounds();

						if(bounds.contains(localPos) && element->_getDepth() < topMostDepth)
						{
							topMostElement = element;
							topMostDepth = element->_getDepth();

							break;
						}
					}
				}
			}
		}

		if(topMostElement != mMouseOverElement)
		{
			if(mMouseOverElement != nullptr)
			{
				// Send MouseOut event
				const RenderWindow* window = mMouseOverWidget->getOwnerWindow();
				Int2 oldScreenPos = Cursor::getWindowPosition(*window);

				GUIMouseEvent event(GUIMouseEventType::MouseOut, oldScreenPos); // TODO - This pos will be wrong as it might be related to completely other window
				mMouseOverElement->mouseEvent(event);
			}

			if(topMostElement != nullptr)
			{
				// Send MouseOver event
				GUIMouseEvent event(GUIMouseEventType::MouseOver, screenPos);
				topMostElement->mouseEvent(event);
			}

			mMouseOverElement = topMostElement;
			mMouseOverWidget = widgetInFocus;
		}

		if(mMouseOverElement != nullptr)
		{
			// Send MouseMove event
			if(mLastCursorPos != screenPos)
			{
				GUIMouseEvent event(GUIMouseEventType::MouseMove, screenPos);
				mMouseOverElement->mouseEvent(event);
			}

			// Send MouseDown and MouseUp events
			for(int i = 0; i < MB_Count; i++)
			{
				bool buttonDown = gInput().isButtonDown((MouseButton)i);
				if(mLastFrameButtonState[i] != buttonDown)
				{
					if(buttonDown)
					{
						GUIMouseEvent event(GUIMouseEventType::MouseDown, screenPos, (MouseButton)i);
						mMouseOverElement->mouseEvent(event);
					}
					else
					{
						GUIMouseEvent event(GUIMouseEventType::MouseUp, screenPos, (MouseButton)i);
						mMouseOverElement->mouseEvent(event);
					}

					mLastFrameButtonState[i] = buttonDown;
				}
			}
		}
	}
}