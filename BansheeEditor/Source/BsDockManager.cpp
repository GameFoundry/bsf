#include "BsDockManager.h"
#include "BsEditorWidgetContainer.h"
#include "BsEditorWidget.h"
#include "BsEditorWidgetManager.h"
#include "BsMath.h"
#include "BsException.h"
#include "BsMesh.h"
#include "BsMaterial.h"
#include "BsVector2.h"
#include "BsDrawList.h"
#include "BsCoreApplication.h"
#include "BsRendererManager.h"
#include "BsCoreRenderer.h"
#include "BsSceneObject.h"
#include "BsGUIManager.h"
#include "BsBuiltinEditorResources.h"
#include "BsGUIWidget.h"
#include "BsCamera.h"
#include "BsDragAndDropManager.h"
#include "BsGUIDockSlider.h"
#include "BsVertexDataDesc.h"
#include "BsGUISkin.h"
#include "BsBuiltinResources.h"
#include "BsDockManagerLayout.h"
#include "BsEditorWindow.h"

#include "BsGUISkin.h"
#include "BsGUIButton.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 DockManager::DockContainer::SLIDER_SIZE = 4;
	const UINT32 DockManager::DockContainer::MIN_CHILD_SIZE = 20;

	const Color DockManager::TINT_COLOR = Color(0.44f, 0.44f, 0.44f, 0.22f);
	const Color DockManager::HIGHLIGHT_COLOR = Color(0.44f, 0.44f, 0.44f, 0.42f);

	DockManager::DockContainer::DockContainer(DockManager* manager)
		:mIsLeaf(true), mWidgets(nullptr), mSplitPosition(0.5f),
		mIsHorizontal(false), mParent(nullptr), mSlider(nullptr), mManager(manager)
	{
		mChildren[0] = nullptr;
		mChildren[1] = nullptr;
	}

	DockManager::DockContainer::DockContainer(DockManager* manager, DockContainer* parent)
		:mIsLeaf(false), mWidgets(nullptr), mSplitPosition(0.5f),
		mIsHorizontal(false), mParent(parent), mSlider(nullptr), mManager(manager)
	{
		mChildren[0] = nullptr;
		mChildren[1] = nullptr;
	}

	DockManager::DockContainer::~DockContainer()
	{
		if(mIsLeaf && mWidgets != nullptr)
			bs_delete(mWidgets);

		if(!mIsLeaf)
		{
			if(mChildren[0] != nullptr)
				bs_delete(mChildren[0]);

			if(mChildren[1] != nullptr)
				bs_delete(mChildren[1]);
		}

		if(mSlider != nullptr)
		{
			GUIElement::destroy(mSlider);
			mSlider = nullptr;
		}
	}

	void DockManager::DockContainer::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
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
			Rect2I clipRect = mArea;
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

				Rect2I elemClipRect(clipRect.x - mArea.x, clipRect.y - mArea.y, clipRect.width, clipRect.height);
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

				Rect2I elemClipRect(clipRect.x - mArea.x, clipRect.y - mArea.y, clipRect.width, clipRect.height);
				mSlider->_setClipRect(elemClipRect);
			}
		}
	}

	void DockManager::DockContainer::makeLeaf(GUIWidget* widgetParent, EditorWindowBase* parentWindow)
	{
		mIsLeaf = true;
		mWidgets = bs_new<EditorWidgetContainer>(widgetParent, parentWindow);

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

	void DockManager::DockContainer::addLeft(EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(false, true);

		mChildren[0]->addWidget(widget);
	}

	void DockManager::DockContainer::addRight(EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(false, false);

		mChildren[1]->addWidget(widget);
	}

	void DockManager::DockContainer::addTop(EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(true, true);

		mChildren[0]->addWidget(widget);
	}

	void DockManager::DockContainer::addBottom(EditorWidgetBase* widget)
	{
		if(mIsLeaf)
			splitContainer(true, false);

		mChildren[1]->addWidget(widget);
	}

	void DockManager::DockContainer::splitContainer(bool horizontal, bool newChildIsFirst, float splitPosition)
	{
		DockContainer* children[2];

		UINT32 idxA = newChildIsFirst ? 0 : 1;
		UINT32 idxB = (idxA + 1) % 2;

		children[idxA] = bs_new<DockContainer>(mManager, this);
		children[idxB] = bs_new<DockContainer>(mManager, this);

		mWidgets->onWidgetClosed.clear();
		
		children[idxA]->makeLeaf(mManager->_getParentWidget(), mManager->mParentWindow);
		children[idxB]->makeLeaf(mWidgets);

		mWidgets = nullptr;
		makeSplit(mManager->_getParentWidget(), children[0], children[1], horizontal, splitPosition);
	}

	void DockManager::DockContainer::makeSplit(GUIWidget* widgetParent, DockManager::DockContainer* first, DockManager::DockContainer* second, bool horizontal, float splitPosition)
	{
		mChildren[0] = first;
		mChildren[1] = second;

		mIsLeaf = false;
		mIsHorizontal = horizontal;
		mSplitPosition = splitPosition;

		if (mWidgets != nullptr)
		{
			bs_delete(mWidgets);
			mWidgets = nullptr;
		}

		if (mSlider != nullptr)
		{
			GUIElement::destroy(mSlider);
			mSlider = nullptr;
		}

		if (horizontal)
		{
			mSlider = GUIDockSlider::create(true, "DockSliderBtn");
			mSlider->_setWidgetDepth(widgetParent->getDepth());
		}
		else
		{
			mSlider = GUIDockSlider::create(false, "DockSliderBtn");
			mSlider->_setWidgetDepth(widgetParent->getDepth());
		}

		mSlider->_changeParentWidget(widgetParent);

		mSlider->onDragged.connect(std::bind(&DockManager::DockContainer::sliderDragged, this, _1));

		setArea(mArea.x, mArea.y, mArea.width, mArea.height);
	}

	void DockManager::DockContainer::addWidget(EditorWidgetBase* widget)
	{
		if(!mIsLeaf)
			return;

		mWidgets->add(*widget);
	}

	void DockManager::DockContainer::addWidget(const String& name)
	{
		if(!mIsLeaf)
			return;

		EditorWidgetManager::instance().create(name, *mWidgets);
	}

	void DockManager::DockContainer::sliderDragged(const Vector2I& delta)
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
				bs_delete(mWidgets);
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

				if (sibling->mIsLeaf)
				{
					sibling->mWidgets->onWidgetClosed.clear();

					mParent->makeLeaf(sibling->mWidgets);
					sibling->mWidgets = nullptr;
				}
				else
				{
					mParent->makeSplit(mManager->_getParentWidget(), sibling->mChildren[0], sibling->mChildren[1], sibling->mIsHorizontal, sibling->mSplitPosition);

					sibling->mChildren[0]->mParent = mParent;
					sibling->mChildren[1]->mParent = mParent;

					sibling->mChildren[0] = nullptr;
					sibling->mChildren[1] = nullptr;
				}

				bs_delete(sibling);
				bs_delete(this);
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

	DockManager::DockContainer* DockManager::DockContainer::findAtPos(const Vector2I& pos)
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

	Rect2I DockManager::DockContainer::getContentBounds() const
	{
		if(!mIsLeaf || mWidgets == nullptr)
			return mArea;

		return mWidgets->getContentBounds();
	}

	void DockManager::DockContainer::update()
	{
		if (mIsLeaf)
		{
			if (mWidgets != nullptr)
				mWidgets->update();
		}
		else
		{
			if (mChildren[0] != nullptr)
				mChildren[0]->update();

			if (mChildren[1] != nullptr)
				mChildren[1]->update();
		}
	}

	DockManager::DockManager(EditorWindowBase* parentWindow, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions), mMouseOverContainer(nullptr), mHighlightedDropLoc(DockLocation::None),
		mShowOverlay(false), mRootContainer(this), mParentWindow(parentWindow)
	{
		mTopDropPolygon = bs_newN<Vector2>(4);
		mBotDropPolygon = bs_newN<Vector2>(4);
		mLeftDropPolygon = bs_newN<Vector2>(4);
		mRightDropPolygon = bs_newN<Vector2>(4);

		mDropOverlayMat = BuiltinEditorResources::instance().createDockDropOverlayMaterial();

		mRenderCallback = RendererManager::instance().getActive()->onRenderViewport.connect(std::bind(&DockManager::render, this, _1, _2));
	}

	DockManager::~DockManager()
	{
		mRenderCallback.disconnect();

		bs_deleteN(mTopDropPolygon, 4);
		bs_deleteN(mBotDropPolygon, 4);
		bs_deleteN(mLeftDropPolygon, 4);
		bs_deleteN(mRightDropPolygon, 4);
	}

	DockManager* DockManager::create(EditorWindowBase* parentWindow)
	{
		return new (bs_alloc<DockManager, PoolAlloc>()) DockManager(parentWindow, GUILayoutOptions::create());
	}

	void DockManager::update()
	{
		if(!DragAndDropManager::instance().isDragInProgress())
		{
			mHighlightedDropLoc = DockLocation::None;
			mShowOverlay = false;
		}

		mRootContainer.update();
	}

	void DockManager::render(const Viewport* viewport, DrawList& drawList)
	{
		if (_getParentWidget() == nullptr || _getParentWidget()->getTarget() != viewport)
			return;

		if(!mShowOverlay)
			return;

		float invViewportWidth = 1.0f / (viewport->getWidth() * 0.5f);
		float invViewportHeight = 1.0f / (viewport->getHeight() * 0.5f);

		if(!mDropOverlayMesh.isLoaded())
			return;

		if(!mDropOverlayMat.isLoaded())
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

		drawList.add(mDropOverlayMat.getInternalPtr(), mDropOverlayMesh.getInternalPtr(), 0, Vector3::ZERO);
	}

	void DockManager::insert(EditorWidgetContainer* relativeTo, EditorWidgetBase* widgetToInsert, DockLocation location)
	{
		if(relativeTo != nullptr)
		{
			DockContainer* container = mRootContainer.find(relativeTo);
			if(container == nullptr)
				BS_EXCEPT(InternalErrorException, "Cannot find the wanted widget container relative to which the widget should be inserted.");

			switch(location)
			{
			case DockLocation::Left:
				container->addLeft(widgetToInsert);
				break;
			case DockLocation::Right:
				container->addRight(widgetToInsert);
				break;
			case DockLocation::Top:
				container->addTop(widgetToInsert);
				break;
			case DockLocation::Bottom:
				container->addBottom(widgetToInsert);
				break;
			}
		}
		else
		{
			if(mRootContainer.mWidgets != nullptr)
				BS_EXCEPT(InternalErrorException, "Trying to insert a widget into dock manager root container but one already exists.");

			mRootContainer.makeLeaf(_getParentWidget(), mParentWindow);
			mRootContainer.addWidget(widgetToInsert);
		}
	}

	void DockManager::setArea(INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		mRootContainer.setArea(x, y, width, height);
		mArea = Rect2I(x, y, width, height);

		updateDropOverlay(x, y, width, height);
	}

	void DockManager::closeAll()
	{
		mRootContainer = DockContainer(this);
		mMouseOverContainer = nullptr;
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
			Vector<String> widgetNames;
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

		DockManagerLayoutPtr layout = bs_shared_ptr<DockManagerLayout>();
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

		Stack<StackElem> todo;
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
						Vector<String> widgetNames = GetWidgetNamesInContainer(currentElem.container->mChildren[i]);
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
		Vector<EditorWidgetBase*> undockedWidgets;

		Stack<DockContainer*> todo;
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

		mRootContainer = DockContainer(this);

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

		auto OpenWidgets = [&] (DockContainer* parent, const Vector<String>& widgetNames)
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
			mRootContainer.makeLeaf(_getParentWidget(), mParentWindow);
			OpenWidgets(&mRootContainer, leafEntry->widgetNames);

			if(!rootEntry->isLeaf)
			{
				Stack<StackEntry> layoutTodo;
				layoutTodo.push(StackEntry(rootEntry, &mRootContainer));

				while(!layoutTodo.empty())
				{
					StackEntry curEntry = layoutTodo.top();
					layoutTodo.pop();

					leafEntry = GetLeafEntry(curEntry.layoutEntry->children[1], 0);

					curEntry.container->splitContainer(curEntry.layoutEntry->horizontalSplit, false, curEntry.layoutEntry->splitPosition);

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
			Stack<StackEntry> layoutTodo;
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

		VertexDataDescPtr vertexDesc = bs_shared_ptr<VertexDataDesc>();
		
		vertexDesc->addVertElem(VET_FLOAT2, VES_POSITION);
		vertexDesc->addVertElem(VET_COLOR, VES_COLOR);

		MeshDataPtr meshData = bs_shared_ptr<MeshData, ScratchAlloc>(16, 24, vertexDesc);

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

			const Matrix4& worldTfrm = _getParentWidget()->SO()->getWorldTfrm();

			Vector4 tfrmdPos = worldTfrm.multiplyAffine(Vector4((float)widgetRelPos.x, (float)widgetRelPos.y, 0.0f, 1.0f));
			Vector2 windowPosVec(tfrmdPos.x, tfrmdPos.y);
			Vector2I windowPos(Math::roundToInt(windowPosVec.x), Math::roundToInt(windowPosVec.y));

			mMouseOverContainer = mRootContainer.findAtPos(windowPos);
			if(mMouseOverContainer == nullptr)
				mMouseOverContainer = &mRootContainer;

			Rect2I overlayBounds;
			
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
			const Matrix4& worldTfrm = _getParentWidget()->SO()->getWorldTfrm();

			Vector4 tfrmdPos = worldTfrm.multiplyAffine(Vector4((float)widgetRelPos.x, (float)widgetRelPos.y, 0.0f, 1.0f));
			Vector2 windowPosVec(tfrmdPos.x, tfrmdPos.y);
			Vector2I windowPos(Math::roundToInt(windowPosVec.x), Math::roundToInt(windowPosVec.y));

			DockContainer* mouseOverContainer = mRootContainer.findAtPos(windowPos);

			if(mouseOverContainer == nullptr)
			{
				Rect2I overlayBounds = mRootContainer.getContentBounds();
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
	bool DockManager::insidePolygon(Vector2* polyPoints, UINT32 numPoints, Vector2 point) const
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