#include "BsGUIResourceTreeView.h"
#include "BsGUISkin.h"
#include "BsProjectLibrary.h"
#include "BsDragAndDropManager.h"
#include "CmResources.h"
#include "CmResourceManifest.h"
#include "BsProjectLibrary.h"
#include "CmFileSystem.h"
#include "BsGUIWidget.h"
#include "CmViewport.h"
#include "CmRenderWindow.h"
#include "CmPlatform.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIResourceTreeView::InternalDraggedResources::InternalDraggedResources(UINT32 numObjects)
		:numObjects(numObjects)
	{
		resourcePaths = cm_newN<WPath>(numObjects);
	}

	GUIResourceTreeView::InternalDraggedResources::~InternalDraggedResources()
	{
		cm_deleteN(resourcePaths, numObjects);
		resourcePaths = nullptr;
	}

	GUIResourceTreeView::GUIResourceTreeView(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, GUIElementStyle* editBoxStyle, 
		BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const GUILayoutOptions& layoutOptions)
		:GUITreeView(parent, backgroundStyle, elementBtnStyle, foldoutBtnStyle, selectionBackgroundStyle, editBoxStyle, dragHighlightStyle,
		dragSepHighlightStyle, layoutOptions), mDraggedResources(nullptr), mCurrentWindow(nullptr), mDropTarget(nullptr), mDropTargetDragActive(false)
	{
		struct StackElem
		{
			StackElem(const ProjectLibrary::LibraryEntry* entry, ResourceTreeElement* treeElem)
				:entry(entry), treeElem(treeElem)
			{ }

			const ProjectLibrary::LibraryEntry* entry;
			ResourceTreeElement* treeElem;
		};

		ProjectLibrary::instance().onEntryAdded.connect(boost::bind(&GUIResourceTreeView::entryAdded, this, _1));
		ProjectLibrary::instance().onEntryRemoved.connect(boost::bind(&GUIResourceTreeView::entryRemoved, this, _1));

		const ProjectLibrary::LibraryEntry* rootEntry = ProjectLibrary::instance().getRootEntry();

		mRootElement.mIsExpanded = true;

		Stack<StackElem>::type todo;
		todo.push(StackElem(rootEntry, &mRootElement));

		while(!todo.empty())
		{
			StackElem curElem = todo.top();
			todo.top();

			const ProjectLibrary::DirectoryEntry* dirEntry = static_cast<const ProjectLibrary::DirectoryEntry*>(curElem.entry);

			for(auto& child : dirEntry->mChildren)
			{
				ResourceTreeElement* newChild = addTreeElement(curElem.treeElem, curElem.entry->path);

				if(child->type == ProjectLibrary::LibraryEntryType::Directory)
					todo.push(StackElem(child, newChild));
			}

			sortTreeElement(curElem.treeElem);
		}

		

		if(parent.getTarget()->getTarget()->isWindow())
		{
			RenderWindow* parentWindow = static_cast<RenderWindow*>(parent.getTarget()->getTarget().get());
			setDropTarget(parentWindow, _getOffset().x, _getOffset().y, _getWidth(), _getHeight());
		}
	}

	GUIResourceTreeView::~GUIResourceTreeView()
	{
		clearDropTarget();
	}

	GUIResourceTreeView* GUIResourceTreeView::create(GUIWidget& parent, GUIElementStyle* backgroundStyle, GUIElementStyle* elementBtnStyle, 
		GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, GUIElementStyle* editBoxStyle, GUIElementStyle* dragHighlightStyle, 
		GUIElementStyle* dragSepHighlightStyle)
	{
		return new (cm_alloc<GUIResourceTreeView, PoolAlloc>()) GUIResourceTreeView(parent, backgroundStyle, elementBtnStyle, foldoutBtnStyle, 
			selectionBackgroundStyle, editBoxStyle, dragHighlightStyle, dragSepHighlightStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIResourceTreeView* GUIResourceTreeView::create(GUIWidget& parent, const GUIOptions& options, GUIElementStyle* backgroundStyle,
		GUIElementStyle* elementBtnStyle, GUIElementStyle* foldoutBtnStyle, GUIElementStyle* selectionBackgroundStyle, 
		GUIElementStyle* editBoxStyle, GUIElementStyle* dragHighlightStyle, GUIElementStyle* dragSepHighlightStyle)
	{
		return new (cm_alloc<GUIResourceTreeView, PoolAlloc>()) GUIResourceTreeView(parent, backgroundStyle, elementBtnStyle, 
			foldoutBtnStyle, selectionBackgroundStyle, editBoxStyle, dragHighlightStyle, dragSepHighlightStyle, GUILayoutOptions::create(options, &GUISkin::DefaultStyle));
	}

	void GUIResourceTreeView::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height, RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		GUITreeView::_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);

		if(mDropTarget != nullptr)
		{
			mDropTarget->setArea(x, y, width, height);
		}
	}

	void GUIResourceTreeView::updateTreeElementHierarchy()
	{
		// Do nothing, updates are handled via callbacks
	}

	void GUIResourceTreeView::renameTreeElement(GUITreeView::TreeElement* element, const CM::WString& name)
	{
		ResourceTreeElement* resourceTreeElement = static_cast<ResourceTreeElement*>(element);
		
		WPath oldPath = resourceTreeElement->mFullPath;
		WPath newPath = PathUtil::combine(PathUtil::parentPath(oldPath), toPath(name));

		ProjectLibrary::instance().moveEntry(oldPath, findUniquePath(newPath));
	}

	GUIResourceTreeView::ResourceTreeElement* GUIResourceTreeView::addTreeElement(ResourceTreeElement* parent, const CM::WPath& fullPath)
	{
		ResourceTreeElement* newChild = cm_new<ResourceTreeElement>();
		newChild->mParent = parent;
		newChild->mName = toString(PathUtil::getFilename(fullPath));
		newChild->mFullPath = fullPath;
		newChild->mSortedIdx = (UINT32)parent->mChildren.size();
		newChild->mIsDirty = true;

		parent->mChildren.push_back(newChild);

		return newChild;
	}

	void GUIResourceTreeView::deleteTreeElement(ResourceTreeElement* element)
	{
		closeTemporarilyExpandedElements(); // In case this element is one of them

		if(element->mIsSelected)
			unselectElement(element);

		Stack<ResourceTreeElement*>::type todo;
		todo.push(element);

		while(!todo.empty())
		{
			ResourceTreeElement* cur = todo.top();
			todo.pop();

			for(auto& child : cur->mChildren)
				todo.push(static_cast<ResourceTreeElement*>(child));
		}

		if(element->mParent != nullptr)
		{
			auto iterFind = std::find(element->mParent->mChildren.begin(), element->mParent->mChildren.end(), element);
			if(iterFind != element->mParent->mChildren.end())
				element->mParent->mChildren.erase(iterFind);
		}

		cm_delete(element);
	}

	void GUIResourceTreeView::sortTreeElement(ResourceTreeElement* element)
	{
		if(element->mChildren.size() > 0)
			quicksortTreeElements(element->mChildren, 0, (INT32)element->mChildren.size() - 1);
	}

	void GUIResourceTreeView::quicksortTreeElements(CM::Vector<TreeElement*>::type& elements, INT32 first, INT32 last)
	{
		if(first < last)
		{
			int pivot = first + std::rand() % (last - first);

			std::swap(elements[pivot]->mSortedIdx, elements[last]->mSortedIdx);
			pivot = last;

			int i = first;
			for(int j = first; j < last; j++)
			{
				INT32 stringCompare = elements[j]->mName.compare(elements[pivot]->mName);
				if(stringCompare < 0)
				{
					std::swap(elements[j]->mSortedIdx, elements[i]->mSortedIdx);
					i++;
				}
			}

			std::swap(elements[last]->mSortedIdx, elements[pivot]->mSortedIdx);

			quicksortTreeElements(elements, first, i - 1);
			quicksortTreeElements(elements, i + 1, last);
		}
	}

	GUIResourceTreeView::ResourceTreeElement* GUIResourceTreeView::findTreeElement(const CM::WPath& fullPath)
	{
		auto pathIter = fullPath.begin();
		auto rootIter = mRootElement.mFullPath.begin();

		while(*pathIter == *rootIter)
		{
			++pathIter;
			++rootIter;
		}

		if(pathIter == fullPath.begin()) // Supplied path not part of the root path
			return nullptr;

		--pathIter;

		Stack<ResourceTreeElement*>::type todo;
		todo.push(&mRootElement);

		while(!todo.empty())
		{
			ResourceTreeElement* current = todo.top();
			todo.pop();

			if(*pathIter == *(--(current->mFullPath.end())))
			{
				for(auto& child : current->mChildren)
					todo.push(static_cast<ResourceTreeElement*>(child));

				if(pathIter == fullPath.end())
					return current;

				++pathIter;
			}
		}

		return nullptr;
	}

	void GUIResourceTreeView::entryAdded(const WPath& path)
	{
		WPath parentPath = PathUtil::parentPath(path);

		ResourceTreeElement* parentElement = findTreeElement(parentPath);
		assert(parentElement != nullptr);

		addTreeElement(parentElement, path);
		sortTreeElement(parentElement);

		markContentAsDirty();
	}

	void GUIResourceTreeView::entryRemoved(const WPath& path)
	{
		ResourceTreeElement* treeElement = findTreeElement(path);
		
		if(treeElement != nullptr)
			deleteTreeElement(treeElement);
	}

	void GUIResourceTreeView::setDropTarget(CM::RenderWindow* parentWindow, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height)
	{
		if(mDropTarget != nullptr)
		{
			Platform::destroyDropTarget(*mDropTarget);

			mDropTargetEnterConn.disconnect();
			mDropTargetLeaveConn.disconnect();
			mDropTargetMoveConn.disconnect();
			mDropTargetDroppedConn.disconnect();
		}

		if(parentWindow != nullptr)
		{
			mCurrentWindow = parentWindow;
			mDropTarget = &Platform::createDropTarget(mCurrentWindow, _getOffset().x, _getOffset().y, _getWidth(), _getHeight());

			mDropTargetEnterConn = mDropTarget->onEnter.connect(boost::bind(&GUIResourceTreeView::dropTargetDragMove, this, _1, _2));
			mDropTargetMoveConn = mDropTarget->onDragOver.connect(boost::bind(&GUIResourceTreeView::dropTargetDragMove, this, _1, _2));
			mDropTargetLeaveConn = mDropTarget->onLeave.connect(boost::bind(&GUIResourceTreeView::dropTargetDragLeave, this));
			mDropTargetDroppedConn = mDropTarget->onDrop.connect(boost::bind(&GUIResourceTreeView::dropTargetDragDropped, this, _1, _2));
		}
		else
			mDropTarget = nullptr;
	}

	void GUIResourceTreeView::clearDropTarget()
	{
		setDropTarget(nullptr, 0, 0, 0, 0);
	}

	void GUIResourceTreeView::dropTargetDragMove(CM::INT32 x, CM::INT32 y)
	{
		mDragPosition = Vector2I(x, y);
		mDragInProgress = true;
		mDropTargetDragActive = true;
		markContentAsDirty();

		if(mBottomScrollBounds.contains(mDragPosition))
		{
			if(mScrollState != ScrollState::Down)
				mScrollState = ScrollState::TransitioningDown;
		}
		else if(mTopScrollBounds.contains(mDragPosition))
		{
			if(mScrollState != ScrollState::Up)
				mScrollState = ScrollState::TransitioningUp;
		}
		else
			mScrollState = ScrollState::None;
	}

	void GUIResourceTreeView::dropTargetDragLeave()
	{
		mDragInProgress = false;
		mDropTargetDragActive = false;
		markContentAsDirty();
	}

	void GUIResourceTreeView::dropTargetDragDropped(CM::INT32 x, CM::INT32 y)
	{
		const GUITreeView::InteractableElement* element = findElementUnderCoord(Vector2I(x, y));

		TreeElement* treeElement = nullptr;
		if(element != nullptr)
		{
			if(element->isTreeElement())
				treeElement = element->getTreeElement();
			else
				treeElement = element->parent;
		}

		if(mDropTarget->getDropType() == OSDropType::FileList)
		{
			Vector<WString>::type fileList = mDropTarget->getFileList();

			mDraggedResources = cm_new<InternalDraggedResources>((UINT32)fileList.size());
			for(UINT32 i = 0; i < (UINT32)fileList.size(); i++)
				mDraggedResources->resourcePaths[i] = toPath(fileList[i]);

			dragAndDropEnded(treeElement);

			cm_delete(mDraggedResources);
			mDraggedResources = nullptr;

			unselectAll();
		}

		mDragInProgress = false;
		mDropTargetDragActive = false;
		markContentAsDirty();
	}

	CM::WPath GUIResourceTreeView::findUniquePath(const CM::WPath& path)
	{
		if(FileSystem::exists(path))
		{
			WPath noExtensionPath = path;
			WPath extension = PathUtil::getExtension(path);
			PathUtil::replaceExtension(noExtensionPath, L"");

			WPath newPath;
			UINT32 cnt = 1;
			do 
			{
				newPath = PathUtil::combine(PathUtil::combine(noExtensionPath, toPath(L" " + toWString(cnt))), extension);
				cnt++;
			} while (FileSystem::exists(newPath));

			return newPath;
		}
		else
			return path;
	}

	bool GUIResourceTreeView::acceptDragAndDrop() const
	{
		return mDropTargetDragActive || DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == (UINT32)DragAndDropType::Resources;
	}

	void GUIResourceTreeView::dragAndDropStart()
	{
		assert(mDraggedResources == nullptr);

		DraggedResources* draggedResources = cm_new<DraggedResources>();
		InternalDraggedResources* internalDraggedResources = cm_new<InternalDraggedResources>((UINT32)mSelectedElements.size());

		ResourceManifestPtr resourceManifest = gResources().getResourceManifest();

		UINT32 cnt = 0;
		for(auto& selectedElement : mSelectedElements)
		{
			ResourceTreeElement* resourceTreeElement = static_cast<ResourceTreeElement*>(selectedElement.element);
			internalDraggedResources->resourcePaths[cnt] = resourceTreeElement->mFullPath; 
			cnt++;

			if(resourceManifest->filePathExists(internalDraggedResources->resourcePaths[cnt]))
				draggedResources->resourceUUIDs.push_back(resourceManifest->filePathToUUID(internalDraggedResources->resourcePaths[cnt]));
		}

		mDraggedResources = internalDraggedResources;

		DragAndDropManager::instance().startDrag(HTexture(), (UINT32)DragAndDropType::Resources, (void*)draggedResources, 
			boost::bind(&GUIResourceTreeView::dragAndDropFinalize, this));
	}

	void GUIResourceTreeView::dragAndDropEnded(TreeElement* overTreeElement)
	{
		if(overTreeElement != nullptr && mDraggedResources != nullptr)
		{
			ResourceTreeElement* resourceTreeElement = static_cast<ResourceTreeElement*>(overTreeElement);

			WPath destDir = resourceTreeElement->mFullPath;
			if(FileSystem::isFile(destDir))
				destDir = PathUtil::parentPath(destDir);

			for(UINT32 i = 0; i < mDraggedResources->numObjects; i++)
			{
				WPath filename = PathUtil::getFilename(mDraggedResources->resourcePaths[i]);

				WPath newPath = PathUtil::combine(destDir, filename);
				ProjectLibrary::instance().moveEntry(mDraggedResources->resourcePaths[i], findUniquePath(newPath));
			}
		}
	}

	void GUIResourceTreeView::dragAndDropFinalize()
	{
		mDragInProgress = false;
		markContentAsDirty();

		DraggedResources* draggedResources = reinterpret_cast<DraggedResources*>(DragAndDropManager::instance().getDragData());
		cm_delete(draggedResources);

		if(mDraggedResources != nullptr)
		{
			cm_delete(mDraggedResources);
			mDraggedResources = nullptr;
		}
	}

	void GUIResourceTreeView::_changeParentWidget(GUIWidget* widget)
	{
		GUITreeView::_changeParentWidget(widget);

		if(widget->getTarget()->getTarget()->isWindow())
		{
			RenderWindow* parentWindow = static_cast<RenderWindow*>(widget->getTarget()->getTarget().get());
			setDropTarget(parentWindow, _getOffset().x, _getOffset().y, _getWidth(), _getHeight());
		}
		else
			clearDropTarget();
	}

	const String& GUIResourceTreeView::getGUITypeName()
	{
		static String typeName = "ResourceTreeView";
		return typeName;
	}
}