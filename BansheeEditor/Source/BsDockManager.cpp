#include "BsDockManager.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWidget.h"
#include "BsEditorWidgetManager.h"
#include "CmMath.h"
#include "CmException.h"
#include "CmMesh.h"
#include "CmMaterial.h"
#include "CmVector2.h"
#include "CmRenderQueue.h"
#include "CmApplication.h"
#include "CmRendererManager.h"
#include "CmRenderer.h"
#include "CmSceneObject.h"
#include "BsGUIManager.h"
#include "BsBuiltinMaterialManager.h"
#include "BsGUIWidget.h"
#include "BsCamera.h"
#include "BsDragAndDropManager.h"
#include "BsGUIDockSlider.h"
#include "CmVertexDataDesc.h"
#include "BsGUISkin.h"
#include "BsBuiltinResources.h"
#include "BsDockManagerLayout.h"

#include "BsGUISkin.h"
#include "BsGUIButton.h"

using namespace CamelotFramework;
using namespace BansheeEngine;
using namespace std::placeholders;

namespace BansheeEditor
{
	const CM::UINT32 DockManager::DockContainer::SLIDER_SIZE = 4;
	const CM::UINT32 DockManager::DockContainer::MIN_CHILD_SIZE = 20;

	const CM::Color DockManager::TINT_COLOR = Color(0.44f, 0.44f, 0.44f, 0.22f);
	const CM::Color DockManager::HIGHLIGHT_COLOR = Color(0.44f, 0.44f, 0.44f, 0.42f);

	DockManager::DockContainer::DockContainer()
		:mIsLeaf(true), mWidgets(nullptr), mSplitPosition(0.5f),
		mIsHorizontal(false), mParent(nullptr), mSlider(nullptr)
	{
		mChildren[0] = nullptr;
		mChildren[1] = nullptr;
	}

	DockManager::DockContainer::DockContainer(DockContainer* parent)
		:mIsLeaf(false), mWidgets(nullptr), mSplitPosition(0.5f),
		mIsHorizontal(false), mParent(parent), mSlider(nullptr)
	{
		mChildren[0] = nullptr;
		mChildren[1] = nullptr;
	}

	DockManager::DockContainer::~DockContainer()
	{
		if(mIsLeaf && mWidgets != nullptr)
			cm_delete(mWidgets);

		if(!mIsLeaf)
		{
			if(mChildren[0] != nullptr)
				cm_delete(mChildren[0]);

			if(mChildren[1] != nullptr)
				cm_delete(mChildren[1]);
		}

		if(mSlider != nullptr)
		{
			GUIElement::destroy(mSlider);
			mSlider = nullptr;
		}
	}

	void DockManager::DockContainer::setArea(CM::INT32 x, CM::INT32 y, UINT32 width, UINT32 height)
	{
		if(mIsLeaf)
		{
			if(mWidgets != nullptr)
			{
				mWidgets->setPosition(x, y);
				mWidgets->setSize(width, height);
			}
		}

		mArea.x = x;
		mArea.y = y;
		mArea.width = width;
		mArea.height = height;

		updateChildAreas();
	}

	void DockManager::DockContainer::updateChildAreas()
	{
		if(!mIsLeaf && mChildren[0] != nullptr && mChildren[1] != nullptr)
		{
			RectI clipRect = mArea;
			if(mIsHorizontal)
			{
				UINT32 remainingSize = (UINT32)std::max(0, (INT32)mArea.height - (INT32)SLIDER_SIZE);
				UINT32 sizeTop = Math::floorToInt(remainingSize * mSplitPosition);
				UINT32 sizeBottom = remainingSize - sizeTop;

				mChildren[0]->setArea(mArea.x, mArea.y, mArea.width, sizeTop);
				mChildren[1]->setArea(mArea.x, mArea.y + sizeTop + SLIDER_SIZE, mArea.width, sizeBottom);

				mSlider->_setOffset(Vector2I(mArea.x, mArea.y + sizeTop));
				mSlider->_setWidth(mArea.width);
				mSlider->_setHeight(SLIDER_SIZE);

				RectI elemClipRect(clipRect.x - mArea.x, clipRect.y - mArea.y, clipRect.width, clipRect.height);
				mSlider->_setClipRect(elemClipRect);
			}
			else
			{
				UINT32 remainingSize = (UINT32)std::max(0, (INT32)mArea.width - (INT32)SLIDER_SIZE);
				UINT32 sizeLeft = Math::floorToInt(remainingSize * mSplitPosition);
				UINT32 sizeRight = remainingSize - sizeLeft;

				mChildren[0]->setArea(mArea.x, mArea.y, sizeLeft, mArea.height);
				mChildren[1]->setArea(mArea.x + sizeLeft + SLIDER_SIZE, mArea.y, sizeRight, mArea.height);

				mSlider->_setOffset(Vector2I(mArea.x + sizeLeft, mArea.y));
				mSlider->_setWidth(SLIDER_SIZE);
				mSlider->_setHeight(mArea.height);

				RectI elemClipRect(clipRect.x - mArea.x, clipRect.y - mArea.y, clipRect.width, clipRect.height);
				mSlider->_setClipRect(elemClipRect);
			}
		}
	}

	void DockManager::DockContainer::makeLeaf(GUIWidget* widgetParent, RenderWindow* parentWindow)
	{
		mIsLeaf = true;
		mWidgets = cm_new<EditorWidgetContainer>(widgetParent, parentWindow, nullptr);

		mWidgets->onWidgetClosed.connect(std::bind(&DockManager::DockContainer::widgetRemoved, this));

		if(mSlider != nullptr)
		{
			GUIElement::destroy(mSlider);
			mSlider = nullptr;
		}

		mWidgets->setPosition(mArea.x, mArea.y);
		mWidgets->setSize(mArea.width, mArea.height);
	}

	void DockManager::DockContainer::makeLeaf(EditorWidgetContainer* existingContainer)
	{
		mIsLeaf = true;
		mWidgets = existingContainer;

		mWidgets->onWidgetClosed.connect(std::bind(&DockManager::DockContainer::widgetRemoved, this));

		if(mSlider != nullptr)
		{
			GUIElement::destroy(mSlider);
			mSlider = nullptr;
		}

		mWidgets->setPosition(mArea.x, mArea.y);
		mWidgets->setSize(mArea.width, mArea.height);
	}

	void DockManager::DockContainer::addLeft(BS::GUIWidget* widgetParent, RenderWindow* parentWindow, EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(widgetParent, parentWindow, false, true);

		mChildren[0]->addWidget(widget);
	}

	void DockManager::DockContainer::addRight(BS::GUIWidget* widgetParent, RenderWindow* parentWindow, EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(widgetParent, parentWindow, false, false);

		mChildren[1]->addWidget(widget);
	}

	void DockManager::DockContainer::addTop(BS::GUIWidget* widgetParent, RenderWindow* parentWindow, EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(widgetParent, parentWindow, true, true);

		mChildren[0]->addWidget(widget);
	}

	void DockManager::DockContainer::addBottom(BS::GUIWidget* widgetParent, RenderWindow* parentWindow, EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(widgetParent, parentWindow, true, false);

		mChildren[1]->addWidget(widget);
	}

	void DockManager::DockContainer::splitContainer(BS::GUIWidget* widgetParent, RenderWindow* parentWindow, bool horizontal, bool newChildIsFirst, float splitPosition)
	{
		UINT32 idxA = newChildIsFirst ? 0 : 1;
		UINT32 idxB = (idxA + 1) % 2;

		mChildren[idxA] = cm_new<DockContainer>(this);
		mChildren[idxB] = cm_new<DockContainer>(this);

		mWidgets->onWidgetClosed.disconnect_all_slots();

		mChildren[idxA]->makeLeaf(widgetParent, parentWindow);
		mChildren[idxB]->makeLeaf(mWidgets);

		mIsLeaf = false;
		mIsHorizontal = horizontal;
		mWidgets = nullptr;
		mSplitPosition = splitPosition;

		if(horizontal)
		{
			mSlider = GUIDockSlider::create(*widgetParent, true, widgetParent->getSkin().getStyle("DockSliderBtn"));
			mSlider->_setWidgetDepth(widgetParent->getDepth());
		}
		else
		{
			mSlider = GUIDockSlider::create(*widgetParent, false, widgetParent->getSkin().getStyle("DockSliderBtn"));
			mSlider->_setWidgetDepth(widgetParent->getDepth());
		}

		mSlider->onDragged.connect(std::bind(&DockManager::DockContainer::sliderDragged, this, _1));

		setArea(mArea.x, mArea.y, mArea.width, mArea.height);
	}

	void DockManager::DockContainer::addWidget(EditorWidgetBase* widget)
	{
		if(!mIsLeaf)
			return;

		mWidgets->add(*widget);
	}

	void DockManager::DockContainer::addWidget(const CM::String& name)
	{
		if(!mIsLeaf)
			return;

		EditorWidgetManager::instance().create(name, *mWidgets);
	}

	void DockManager::DockContainer::sliderDragged(const CM::Vector2I& delta)
	{
		if(mIsHorizontal && delta.y != 0)
		{
			UINT32 maxSize = (UINT32)std::max(MIN_CHILD_SIZE, (INT32)mArea.height - (INT32)SLIDER_SIZE - MIN_CHILD_SIZE);
			UINT32 remainingSize = (UINT32)std::max(0, (INT32)mArea.height - (INT32)SLIDER_SIZE);
			mSplitPosition = Math::clamp((UINT32)Math::floorToInt(remainingSize * mSplitPosition) + delta.y, MIN_CHILD_SIZE, maxSize) / (float)remainingSize;

			updateChildAreas();
		}
		else if(!mIsHorizontal && delta.x != 0)
		{
			UINT32 maxSize = (UINT32)std::max(MIN_CHILD_SIZE, (INT32)mArea.width - (INT32)SLIDER_SIZE - MIN_CHILD_SIZE);
			UINT32 remainingSize = (UINT32)std::max(0, (INT32)mArea.width - (INT32)SLIDER_SIZE);
			mSplitPosition = Math::clamp((UINT32)Math::floorToInt(remainingSize * mSplitPosition) + delta.x, MIN_CHILD_SIZE, maxSize) / (float)remainingSize;

			updateChildAreas();
		}
	}

	void DockManager::DockContainer::widgetRemoved()
	{
		assert(mIsLeaf);

		if(mWidgets->getNumWidgets() == 0)
		{
			if(mParent == nullptr) // We're root so we just reset ourselves, can't delete root
			{
				cm_delete(mWidgets);
				mWidgets = nullptr;

				mIsLeaf = false;
				mSplitPosition = 0.5f;
				mIsHorizontal = false;
			}
			else
			{
				// Replace our parent with our sibling
				DockContainer* sibling = nullptr;
				if(mParent->mChildren[0] == this)
					sibling = mParent->mChildren[1];
				else
					sibling = mParent->mChildren[0];

				sibling->mWidgets->onWidgetClosed.disconnect_all_slots();

				mParent->makeLeaf(sibling->mWidgets);
				sibling->mWidgets = nullptr;

				cm_delete(sibling);
				cm_delete(this);
			}
		}
	}

	DockManager::DockContainer* DockManager::DockContainer::find(EditorWidgetContainer* widgetContainer)
	{
		if(mIsLeaf)
		{
			if(mWidgets == widgetContainer)
				return this;
			else
				return nullptr;
		}
		else
		{
			if(mChildren[0] != nullptr && mChildren[0]->find(widgetContainer) != nullptr)
				return mChildren[0];

			if(mChildren[1] != nullptr && mChildren[1]->find(widgetContainer) != nullptr)
				return mChildren[1];
		}

		return nullptr;
	}

	DockManager::DockContainer* DockManager::DockContainer::findAtPos(const CM::Vector2I& pos)
	{
		if(mIsLeaf)
		{
			if(mArea.contains(pos))
			{
				return this;
			}
		}
		else
		{
			if(mChildren[0] != nullptr && mChildren[0]->findAtPos(pos) != nullptr)
				return mChildren[0];

			if(mChildren[1] != nullptr && mChildren[1]->findAtPos(pos) != nullptr)
				return mChildren[1];
		}

		return nullptr;
	}

	RectI DockManager::DockContainer::getContentBounds() const
	{
		if(!mIsLeaf || mWidgets == nullptr)
			return mArea;

		return mWidgets->getContentBounds();
	}

	DockManager::DockManager(BS::GUIWidget& parent, CM::RenderWindow* parentWindow, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mParentWindow(parentWindow), mMouseOverContainer(nullptr), mHighlightedDropLoc(DockLocation::None),
		mShowOverlay(false)
	{
		mTopDropPolygon = cm_newN<Vector2>(4);
		mBotDropPolygon = cm_newN<Vector2>(4);
		mLeftDropPolygon = cm_newN<Vector2>(4);
		mRightDropPolygon = cm_newN<Vector2>(4);

		mDropOverlayMat = BuiltinMaterialManager::instance().createDockDropOverlayMaterial();

		RendererManager::instance().getActive()->addRenderCallback(mParent->getTarget(), std::bind(&DockManager::render, this, _1, _2));
	}

	DockManager::~DockManager()
	{
		cm_deleteN(mTopDropPolygon, 4);
		cm_deleteN(mBotDropPolygon, 4);
		cm_deleteN(mLeftDropPolygon, 4);
		cm_deleteN(mRightDropPolygon, 4);
	}

	DockManager* DockManager::create(GUIWidget& parent, RenderWindow* parentWindow)
	{
		return new (cm_alloc<DockManager, PoolAlloc>()) DockManager(parent, parentWindow, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	void DockManager::update()
	{
		if(!DragAndDropManager::instance().isDragInProgress())
		{
			mHighlightedDropLoc = DockLocation::None;
			mShowOverlay = false;
		}
	}

	void DockManager::render(const Viewport* viewport, CM::RenderQueue& renderQueue)
	{
		if(!mShowOverlay)
			return;

		float invViewportWidth = 1.0f / (viewport->getWidth() * 0.5f);
		float invViewportHeight = 1.0f / (viewport->getHeight() * 0.5f);

		if(mDropOverlayMesh == nullptr || !mDropOverlayMesh.isLoaded() || !mDropOverlayMesh->isInitialized())
			return;

		if(mDropOverlayMat == nullptr || !mDropOverlayMat.isLoaded() || !mDropOverlayMat->isInitialized())
			return;

		mDropOverlayMat->setFloat("invViewportWidth", invViewportWidth);
		mDropOverlayMat->setFloat("invViewportHeight", invViewportHeight);

		mDropOverlayMat->setColor("tintColor", TINT_COLOR);
		mDropOverlayMat->setColor("highlightColor", HIGHLIGHT_COLOR);

		Color highlightColor;
		switch(mHighlightedDropLoc)
		{
		case DockLocation::Top:
			highlightColor = Color(1.0f, 0.0f, 0.0f, 0.0f);
			break;
		case DockLocation::Bottom:
			highlightColor = Color(0.0f, 1.0f, 0.0f, 0.0f);
			break;
		case DockLocation::Left:
			highlightColor = Color(0.0f, 0.0f, 1.0f, 0.0f);
			break;
		case DockLocation::Right:
			highlightColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case DockLocation::None:
			highlightColor = Color(0.0f, 0.0f, 0.0f, 0.0f);
			break;
		}

		mDropOverlayMat->setColor("highlightActive", highlightColor);

		renderQueue.add(mDropOverlayMat.getInternalPtr(), mDropOverlayMesh.getInternalPtr(), 0, Vector3::ZERO);
	}

	void DockManager::insert(EditorWidgetContainer* relativeTo, EditorWidgetBase* widgetToInsert, DockLocation location)
	{
		if(relativeTo != nullptr)
		{
			DockContainer* container = mRootContainer.find(relativeTo);
			if(container == nullptr)
				CM_EXCEPT(InternalErrorException, "Cannot find the wanted widget container relative to which the widget should be inserted.");

			switch(location)
			{
			case DockLocation::Left:
				container->addLeft(mParent, mParentWindow, widgetToInsert);
				break;
			case DockLocation::Right:
				container->addRight(mParent, mParentWindow, widgetToInsert);
				break;
			case DockLocation::Top:
				container->addTop(mParent, mParentWindow, widgetToInsert);
				break;
			case DockLocation::Bottom:
				container->addBottom(mParent, mParentWindow, widgetToInsert);
				break;
			}
		}
		else
		{
			if(mRootContainer.mWidgets != nullptr)
				CM_EXCEPT(InternalErrorException, "Trying to insert a widget into dock manager root container but one already exists.");

			mRootContainer.makeLeaf(mParent, mParentWindow);
			mRootContainer.addWidget(widgetToInsert);
		}
	}

	void DockManager::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mRootContainer.setArea(x, y, width, height);
		mArea = RectI(x, y, width, height);

		updateDropOverlay(x, y, width, height);
	}

	DockManagerLayoutPtr DockManager::getLayout() const
	{
		struct StackElem
		{
			StackElem(DockManagerLayout::Entry* layoutEntry, const DockContainer* container)
				:layoutEntry(layoutEntry), container(container)
			{ }

			DockManagerLayout::Entry* layoutEntry;
			const DockContainer* container;
		};

		auto GetWidgetNamesInContainer = [&] (const DockContainer* container)
		{
			Vector<String>::type widgetNames;
			if(container->mWidgets != nullptr)
			{
				UINT32 numWidgets = container->mWidgets->getNumWidgets();

				for(UINT32 i = 0; i < numWidgets; i++)
				{
					EditorWidgetBase* widget = container->mWidgets->getWidget(i);
					widgetNames.push_back(widget->getName());
				}				
			}

			return widgetNames;
		};

		DockManagerLayoutPtr layout = cm_shared_ptr<DockManagerLayout>();
		DockManagerLayout::Entry* rootEntry = &layout->getRootEntry();

		if(mRootContainer.mIsLeaf)
		{
			rootEntry->isLeaf = true;
			rootEntry->widgetNames = GetWidgetNamesInContainer(&mRootContainer);
		}
		else
		{
			rootEntry->isLeaf = false;
			rootEntry->horizontalSplit = mRootContainer.mIsHorizontal;
			rootEntry->splitPosition = mRootContainer.mSplitPosition;
			rootEntry->parent = nullptr;
		}

		Stack<StackElem>::type todo;
		todo.push(StackElem(rootEntry, &mRootContainer));

		while(!todo.empty())
		{
			StackElem currentElem = todo.top();
			todo.pop();

			if(!currentElem.container->mIsLeaf)
			{
				for(UINT32 i = 0; i < 2; i++)
				{
					if(currentElem.container->mChildren[i] == nullptr)
						continue;

					if(currentElem.container->mChildren[i]->mIsLeaf)
					{
						Vector<String>::type widgetNames = GetWidgetNamesInContainer(currentElem.container->mChildren[i]);
						currentElem.layoutEntry->children[i] = 
							DockManagerLayout::Entry::createLeaf(currentElem.layoutEntry, i, widgetNames);
					}
					else
					{
						currentElem.layoutEntry->children[i] = 
							DockManagerLayout::Entry::createContainer(currentElem.layoutEntry, i, 
							currentElem.container->mChildren[i]->mSplitPosition, 
							currentElem.container->mChildren[i]->mIsHorizontal);

						todo.push(StackElem(currentElem.layoutEntry->children[i], currentElem.container->mChildren[i]));
					}
				}
			}
		}

		return layout;
	}

	void DockManager::setLayout(const DockManagerLayoutPtr& layout)
	{
		// Undock all currently docked widgets
		Vector<EditorWidgetBase*>::type undockedWidgets;

		Stack<DockContainer*>::type todo;
		todo.push(&mRootContainer);

		while(!todo.empty())
		{
			DockContainer* current = todo.top();
			todo.pop();

			if(current->mIsLeaf)
			{
				if(current->mWidgets != nullptr)
				{
					while(current->mWidgets->getNumWidgets() > 0)
					{
						EditorWidgetBase* curWidget = current->mWidgets->getWidget(0);
						current->mWidgets->remove(*curWidget);

						undockedWidgets.push_back(curWidget);
					}
				}
			}
			else
			{
				todo.push(current->mChildren[0]);
				todo.push(current->mChildren[1]);
			}
		}

		mRootContainer = DockContainer();

		// Load layout
		struct StackEntry
		{
			StackEntry(const DockManagerLayout::Entry* layoutEntry, DockContainer* container)
				:layoutEntry(layoutEntry), container(container)
			{ }

			const DockManagerLayout::Entry* layoutEntry;
			DockContainer* container;
		};

		auto GetLeafEntry = [] (const DockManagerLayout::Entry* parentEntry, UINT32 childIdx) -> const DockManagerLayout::Entry*
		{
			while(true)
			{
				if(parentEntry->isLeaf)
					return parentEntry;

				parentEntry = parentEntry->children[childIdx];
			}

			return nullptr;
		};

		auto OpenWidgets = [&] (DockContainer* parent, const Vector<String>::type& widgetNames)
		{
			for(auto& widgetName : widgetNames)
			{
				parent->addWidget(widgetName);
			}
		};

		// Dock elements
		const DockManagerLayout::Entry* rootEntry = &layout->getRootEntry();
		const DockManagerLayout::Entry* leafEntry = GetLeafEntry(rootEntry, 0);

		if(leafEntry->widgetNames.size() > 0) // If zero, entire layout is empty
		{
			mRootContainer.makeLeaf(mParent, mParentWindow);
			OpenWidgets(&mRootContainer, leafEntry->widgetNames);

			if(!rootEntry->isLeaf)
			{
				Stack<StackEntry>::type layoutTodo;
				layoutTodo.push(StackEntry(rootEntry, &mRootContainer));

				while(!layoutTodo.empty())
				{
					StackEntry curEntry = layoutTodo.top();
					layoutTodo.pop();

					leafEntry = GetLeafEntry(curEntry.layoutEntry->children[1], 0);

					curEntry.container->splitContainer(mParent, mParentWindow, curEntry.layoutEntry->horizontalSplit, false, curEntry.layoutEntry->splitPosition);

					DockContainer* otherChild = curEntry.container->mChildren[1];
					OpenWidgets(otherChild, leafEntry->widgetNames);

					if(!curEntry.layoutEntry->children[0]->isLeaf)
						layoutTodo.push(StackEntry(curEntry.layoutEntry->children[0], curEntry.container->mChildren[0]));

					if(!curEntry.layoutEntry->children[1]->isLeaf)
						layoutTodo.push(StackEntry(curEntry.layoutEntry->children[1], curEntry.container->mChildren[1]));
				}
			}
		}

		// Set container sizes
		{
			Stack<StackEntry>::type layoutTodo;
			layoutTodo.push(StackEntry(rootEntry, &mRootContainer));

			while(!layoutTodo.empty())
			{
				StackEntry curEntry = layoutTodo.top();
				layoutTodo.pop();

				if(!curEntry.layoutEntry->isLeaf)
				{
					layoutTodo.push(StackEntry(curEntry.layoutEntry->children[0], curEntry.container->mChildren[0]));
					layoutTodo.push(StackEntry(curEntry.layoutEntry->children[1], curEntry.container->mChildren[1]));
				}
			}
		}

		// Destroy any widgets that are no longer docked anywhere
		for(auto& widget : undockedWidgets)
		{
			if(widget->_getParent() == nullptr)
				widget->close();
		}

		setArea(mArea.x, mArea.y, mArea.width, mArea.height);
	}

	void DockManager::updateClippedBounds()
	{
		// TODO - Clipping not actually accounted for but shouldn't matter as right now DockManager is only used in one specific situation
		mClippedBounds = mRootContainer.mArea;
	}

	void DockManager::updateDropOverlay(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		const static int spacing = 10;
		const static float innerScale = 0.75f;

		UINT32 outWidth = std::max(0, (INT32)width - spacing * 2);
		UINT32 outHeight = std::max(0, (INT32)height - spacing * 2);
		UINT32 inWidth = (UINT32)Math::floorToInt(innerScale * outWidth);
		UINT32 inHeight = (UINT32)Math::floorToInt(innerScale * outHeight);
		INT32 inXOffset = Math::floorToInt((outWidth - inWidth) * 0.5f);
		INT32 inYOffset = Math::floorToInt((outHeight - inHeight) * 0.5f);

		Vector2 outTopLeft((float)x, (float)y);
		Vector2 outTopRight((float)(x + outWidth), (float)y);
		Vector2 outBotLeft((float)x, (float)(y + outHeight));
		Vector2 outBotRight((float)(x + outWidth), (float)(y + outHeight));

		Vector2 inTopLeft((float)(x + inXOffset), (float)(y + inYOffset));
		Vector2 inTopRight((float)(x + inXOffset + inWidth), (float)(y + inYOffset));
		Vector2 inBotLeft((float)(x + inXOffset), (float)(y + inYOffset + inHeight));
		Vector2 inBotRight((float)(x + inXOffset + inWidth), (float)(y + inYOffset + inHeight));

		VertexDataDescPtr vertexDesc = cm_shared_ptr<VertexDataDesc>();
		
		vertexDesc->addVertElem(VET_FLOAT2, VES_POSITION);
		vertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		MeshDataPtr meshData = cm_shared_ptr<MeshData, ScratchAlloc>(16, 24, vertexDesc);

		auto vertIter = meshData->getVec2DataIter(VES_POSITION);
		auto colIter = meshData->getDWORDDataIter(VES_COLOR);

		// Top
		Vector2 topOffset((float)spacing, 0.0f);

		mTopDropPolygon[0] = outTopLeft + topOffset;
		mTopDropPolygon[1] = outTopRight + topOffset;
		mTopDropPolygon[2] = inTopRight + topOffset;
		mTopDropPolygon[3] = inTopLeft + topOffset;

		vertIter.addValue(mTopDropPolygon[0]);
		vertIter.addValue(mTopDropPolygon[1]);
		vertIter.addValue(mTopDropPolygon[2]);
		vertIter.addValue(mTopDropPolygon[3]);

		Color color(1.0f, 0.0f, 0.0f, 0.0f);
		UINT32 color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		// Bottom
		Vector2 botOffset((float)spacing, (float)spacing * 2.0f);

		mBotDropPolygon[0] = inBotLeft + botOffset;
		mBotDropPolygon[1] = inBotRight + botOffset;
		mBotDropPolygon[2] = outBotRight + botOffset;
		mBotDropPolygon[3] = outBotLeft + botOffset;

		vertIter.addValue(mBotDropPolygon[0]);
		vertIter.addValue(mBotDropPolygon[1]);
		vertIter.addValue(mBotDropPolygon[2]);
		vertIter.addValue(mBotDropPolygon[3]);

		color = Color(0.0f, 1.0f, 0.0f, 0.0f);
		color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		// Left
		Vector2 leftOffset(0.0f, (float)spacing);

		mLeftDropPolygon[0] = outTopLeft + leftOffset;
		mLeftDropPolygon[1] = inTopLeft + leftOffset;
		mLeftDropPolygon[2] = inBotLeft + leftOffset;
		mLeftDropPolygon[3] = outBotLeft + leftOffset;

		vertIter.addValue(mLeftDropPolygon[0]);
		vertIter.addValue(mLeftDropPolygon[1]);
		vertIter.addValue(mLeftDropPolygon[2]);
		vertIter.addValue(mLeftDropPolygon[3]);

		color = Color(0.0f, 0.0f, 1.0f, 0.0f);
		color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		// Right
		Vector2 rightOffset((float)spacing * 2.0f, (float)spacing);

		mRightDropPolygon[0] = inTopRight + rightOffset;
		mRightDropPolygon[1] = outTopRight + rightOffset;
		mRightDropPolygon[2] = outBotRight + rightOffset;
		mRightDropPolygon[3] = inBotRight + rightOffset;

		vertIter.addValue(mRightDropPolygon[0]);
		vertIter.addValue(mRightDropPolygon[1]);
		vertIter.addValue(mRightDropPolygon[2]);
		vertIter.addValue(mRightDropPolygon[3]);

		color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		color32 = color.getAsRGBA();
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);
		colIter.addValue(color32);

		UINT32* indexData = meshData->getIndices32();

		// Top
		indexData[0] = 0;
		indexData[1] = 1;
		indexData[2] = 2;

		indexData[3] = 0;
		indexData[4] = 2;
		indexData[5] = 3;

		// Bottom
		indexData[6] = 4;
		indexData[7] = 5;
		indexData[8] = 6;

		indexData[9] = 4;
		indexData[10] = 6;
		indexData[11] = 7;

		// Left
		indexData[12] = 8;
		indexData[13] = 9;
		indexData[14] = 10;

		indexData[15] = 8;
		indexData[16] = 10;
		indexData[17] = 11;

		// Right
		indexData[18] = 12;
		indexData[19] = 13;
		indexData[20] = 14;

		indexData[21] = 12;
		indexData[22] = 14;
		indexData[23] = 15;

		mDropOverlayMesh = Mesh::create(meshData);
	}

	bool DockManager::mouseEvent(const GUIMouseEvent& event)
	{
		if(event.getType() == GUIMouseEventType::MouseDragAndDropDragged)
		{
			if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
				return false;

			const Vector2I& widgetRelPos = event.getPosition();

			const Matrix4& worldTfrm = mParent->SO()->getWorldTfrm();

			Vector4 tfrmdPos = worldTfrm.multiply3x4(Vector4((float)widgetRelPos.x, (float)widgetRelPos.y, 0.0f, 1.0f));
			Vector2 windowPosVec(tfrmdPos.x, tfrmdPos.y);
			Vector2I windowPos(Math::roundToInt(windowPosVec.x), Math::roundToInt(windowPosVec.y));

			mMouseOverContainer = mRootContainer.findAtPos(windowPos);
			if(mMouseOverContainer == nullptr)
				mMouseOverContainer = &mRootContainer;

			RectI overlayBounds;
			
			if(mMouseOverContainer != nullptr)
				overlayBounds = mMouseOverContainer->getContentBounds();

			// Update mesh if needed
			if(mLastOverlayBounds != overlayBounds)
			{
				if(overlayBounds.width <= 0 || overlayBounds.height <= 0)
					mDropOverlayMesh = HMesh();
				else
					updateDropOverlay(overlayBounds.x, overlayBounds.y, overlayBounds.width, overlayBounds.height);

				mLastOverlayBounds = overlayBounds;
			}

			// Check if we need to highlight any drop locations
			if(mMouseOverContainer != nullptr)
			{
				if(insidePolygon(mTopDropPolygon, 4, windowPosVec))
					mHighlightedDropLoc = DockLocation::Top;
				else if(insidePolygon(mBotDropPolygon, 4, windowPosVec))
					mHighlightedDropLoc = DockLocation::Bottom;
				else if(insidePolygon(mLeftDropPolygon, 4, windowPosVec))
					mHighlightedDropLoc = DockLocation::Left;
				else if(insidePolygon(mRightDropPolygon, 4, windowPosVec))
					mHighlightedDropLoc = DockLocation::Right;
				else
					mHighlightedDropLoc = DockLocation::None;

				if(overlayBounds.contains(windowPos))
					mShowOverlay = true;
				else
					mShowOverlay = false;
			}
			else
				mShowOverlay = false;

			return true;
		}
		else if(event.getType() == GUIMouseEventType::MouseDragAndDropDropped)
		{
			if(DragAndDropManager::instance().getDragTypeId() != (UINT32)DragAndDropType::EditorWidget)
				return false;

			EditorWidgetBase* draggedWidget = reinterpret_cast<EditorWidgetBase*>(DragAndDropManager::instance().getDragData());

			const Vector2I& widgetRelPos = event.getPosition();
			const Matrix4& worldTfrm = mParent->SO()->getWorldTfrm();

			Vector4 tfrmdPos = worldTfrm.multiply3x4(Vector4((float)widgetRelPos.x, (float)widgetRelPos.y, 0.0f, 1.0f));
			Vector2 windowPosVec(tfrmdPos.x, tfrmdPos.y);
			Vector2I windowPos(Math::roundToInt(windowPosVec.x), Math::roundToInt(windowPosVec.y));

			DockContainer* mouseOverContainer = mRootContainer.findAtPos(windowPos);

			if(mouseOverContainer == nullptr)
			{
				RectI overlayBounds = mRootContainer.getContentBounds();
				if(overlayBounds.contains(windowPos))
				{
					insert(nullptr, draggedWidget, DockLocation::None);
				}
			}
			else
			{
				if(insidePolygon(mTopDropPolygon, 4, windowPosVec))
					insert(mouseOverContainer->mWidgets, draggedWidget, DockLocation::Top);
				else if(insidePolygon(mBotDropPolygon, 4, windowPosVec))
					insert(mouseOverContainer->mWidgets, draggedWidget, DockLocation::Bottom);
				else if(insidePolygon(mLeftDropPolygon, 4, windowPosVec))
					insert(mouseOverContainer->mWidgets, draggedWidget, DockLocation::Left);
				else if(insidePolygon(mRightDropPolygon, 4, windowPosVec))
					insert(mouseOverContainer->mWidgets, draggedWidget, DockLocation::Right);
			}

			return true;
		}

		return false;
	}

	// TODO - Move to a separate Polygon class?
	bool DockManager::insidePolygon(CM::Vector2* polyPoints, CM::UINT32 numPoints, CM::Vector2 point) const
	{
		bool isInside = false;
		for (UINT32 i = 0, j = numPoints - 1; i < numPoints; j = i++) 
		{
			float lineVal = (polyPoints[j].x - polyPoints[i].x) * (point.y - polyPoints[i].y) / (polyPoints[j].y - polyPoints[i].y) + polyPoints[i].x;

			if (((polyPoints[i].y > point.y) != (polyPoints[j].y > point.y)) && (point.x < lineVal))
				isInside = !isInside;
		}

		return isInside;
	}
}