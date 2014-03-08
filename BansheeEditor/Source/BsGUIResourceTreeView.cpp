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
#include "CmPath.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	GUIResourceTreeView::InternalDraggedResources::InternalDraggedResources(UINT32 numObjects)
		:numObjects(numObjects)
	{
		resourcePaths = cm_newN<WString>(numObjects);
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
		ProjectLibrary::instance().onEntryAdded.connect(boost::bind(&GUIResourceTreeView::entryAdded, this, _1));
		ProjectLibrary::instance().onEntryRemoved.connect(boost::bind(&GUIResourceTreeView::entryRemoved, this, _1));

		const ProjectLibrary::LibraryEntry* rootEntry = ProjectLibrary::instance().getRootEntry();

		mRootElement.mFullPath = rootEntry->path;
		mRootElement.mElementName = Path::getFilename(mRootElement.mFullPath);
		expandElement(&mRootElement);

		updateFromProjectLibraryEntry(&mRootElement, rootEntry);

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
		
		WString oldPath = resourceTreeElement->mFullPath;
		WString newPath = Path::combine(Path::parentPath(oldPath), name);

		ProjectLibrary::instance().moveEntry(oldPath, findUniquePath(newPath));
	}

	void GUIResourceTreeView::deleteTreeElement(TreeElement* element) 
	{
		ResourceTreeElement* resourceTreeElement = static_cast<ResourceTreeElement*>(element);

		ProjectLibrary::instance().deleteEntry(resourceTreeElement->mFullPath);
	}

	void GUIResourceTreeView::updateFromProjectLibraryEntry(ResourceTreeElement* treeElement, const ProjectLibrary::LibraryEntry* libraryEntry)
	{
		struct StackElem
		{
			StackElem(const ProjectLibrary::LibraryEntry* entry, ResourceTreeElement* treeElem)
				:entry(entry), treeElem(treeElem)
			{ }

			const ProjectLibrary::LibraryEntry* entry;
			ResourceTreeElement* treeElem;
		};

		if(libraryEntry->type == ProjectLibrary::LibraryEntryType::Directory)
		{
			Stack<StackElem>::type todo;
			todo.push(StackElem(libraryEntry, treeElement));

			while(!todo.empty())
			{
				StackElem curElem = todo.top();
				todo.pop();

				const ProjectLibrary::DirectoryEntry* dirEntry = static_cast<const ProjectLibrary::DirectoryEntry*>(curElem.entry);

				for(auto& child : dirEntry->mChildren)
				{
					ResourceTreeElement* newChild = addTreeElement(curElem.treeElem, child->path);

					if(child->type == ProjectLibrary::LibraryEntryType::Directory)
						todo.push(StackElem(child, newChild));
				}

				sortTreeElement(curElem.treeElem);
			}
		}
	}

	GUIResourceTreeView::ResourceTreeElement* GUIResourceTreeView::addTreeElement(ResourceTreeElement* parent, const CM::WString& fullPath)
	{
		ResourceTreeElement* newChild = cm_new<ResourceTreeElement>();
		newChild->mParent = parent;
		newChild->mName = toString(Path::getFilename(fullPath));
		newChild->mFullPath = fullPath;
		newChild->mSortedIdx = (UINT32)parent->mChildren.size();
		newChild->mIsVisible = parent->mIsVisible && parent->mIsExpanded;
		newChild->mElementName = Path::getFilename(fullPath);

		parent->mChildren.push_back(newChild);

		updateElementGUI(parent);
		updateElementGUI(newChild);

		return newChild;
	}

	void GUIResourceTreeView::deleteTreeElement(ResourceTreeElement* element)
	{
		closeTemporarilyExpandedElements(); // In case this element is one of them

		if(element->mIsSelected)
			unselectElement(element);

		if(element->mParent != nullptr)
		{
			auto iterFind = std::find(element->mParent->mChildren.begin(), element->mParent->mChildren.end(), element);
			if(iterFind != element->mParent->mChildren.end())
				element->mParent->mChildren.erase(iterFind);

			sortTreeElement(static_cast<ResourceTreeElement*>(element->mParent));
			updateElementGUI(element->mParent);
		}

		if(&mRootElement != element)
			cm_delete(element);
	}

	void GUIResourceTreeView::sortTreeElement(ResourceTreeElement* element)
	{
		auto cmp = [&] (const TreeElement* a, const TreeElement* b)
		{
			return a->mName.compare(b->mName) < 0;
		};

		std::sort(element->mChildren.begin(), element->mChildren.end(), cmp);

		UINT32 idx = 0;
		for(auto& child : element->mChildren)
		{
			child->mSortedIdx = idx;
			idx++;
		}
	}

	GUIResourceTreeView::ResourceTreeElement* GUIResourceTreeView::findTreeElement(const CM::WString& fullPath)
	{
		Vector<WString>::type pathElems = Path::split(fullPath);
		Vector<WString>::type rootElems = Path::split(mRootElement.mFullPath);

		auto pathIter = pathElems.begin();
		auto rootIter = rootElems.begin();

		while(pathIter != pathElems.end() && rootIter != rootElems.end() && Path::comparePathElements(*pathIter, *rootIter))
		{
			++pathIter;
			++rootIter;
		}

		if(pathIter == pathElems.begin()) // Supplied path not part of the root path
			return nullptr;

		--pathIter;

		Stack<ResourceTreeElement*>::type todo;
		todo.push(&mRootElement);

		while(!todo.empty())
		{
			ResourceTreeElement* current = todo.top();
			todo.pop();

			if(Path::comparePathElements(*pathIter, current->mElementName))
			{
				++pathIter;

				if(pathIter == pathElems.end())
					return current;

				while(!todo.empty())
					todo.pop();

				for(auto& child : current->mChildren)
					todo.push(static_cast<ResourceTreeElement*>(child));
			}
		}

		return nullptr;
	}

	void GUIResourceTreeView::entryAdded(const WString& path)
	{
		WString parentPath = Path::parentPath(path);

		ResourceTreeElement* parentElement = findTreeElement(parentPath);
		assert(parentElement != nullptr);

		ResourceTreeElement* newElement = addTreeElement(parentElement, path);
		sortTreeElement(parentElement);

		ProjectLibrary::LibraryEntry* libEntry = ProjectLibrary::instance().findEntry(path);
		
		assert(libEntry != nullptr);
		updateFromProjectLibraryEntry(newElement, libEntry);

		markContentAsDirty();
	}

	void GUIResourceTreeView::entryRemoved(const WString& path)
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
				mDraggedResources->resourcePaths[i] = fileList[i];

			dragAndDropEnded(treeElement);

			cm_delete(mDraggedResources);
			mDraggedResources = nullptr;

			unselectAll();
		}

		mDragInProgress = false;
		mDropTargetDragActive = false;
		markContentAsDirty();
	}

	CM::WString GUIResourceTreeView::findUniquePath(const CM::WString& path)
	{
		if(FileSystem::exists(path))
		{
			WString noExtensionPath = path;
			WString extension = Path::getExtension(path);
			Path::replaceExtension(noExtensionPath, L"");

			WString newPath;
			UINT32 cnt = 1;
			do 
			{
				newPath = Path::combine(Path::combine(noExtensionPath, L" " + toWString(cnt)), extension);
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

		UINT32 cnt = 0;
		for(auto& selectedElement : mSelectedElements)
		{
			ResourceTreeElement* resourceTreeElement = static_cast<ResourceTreeElement*>(selectedElement.element);
			internalDraggedResources->resourcePaths[cnt] = resourceTreeElement->mFullPath; 

			String uuid;
			if(gResources().getUUIDFromFilePath(internalDraggedResources->resourcePaths[cnt], uuid))
				draggedResources->resourceUUIDs.push_back(uuid);

			cnt++;
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

			WString destDir = resourceTreeElement->mFullPath;
			if(FileSystem::isFile(destDir))
				destDir = Path::parentPath(destDir);

			for(UINT32 i = 0; i < mDraggedResources->numObjects; i++)
			{
				WString filename = Path::getFilename(mDraggedResources->resourcePaths[i]);
				WString currentParent = Path::parentPath(mDraggedResources->resourcePaths[i]);

				if(!Path::equals(currentParent, destDir))
				{
					WString newPath = Path::combine(destDir, filename);
					ProjectLibrary::instance().moveEntry(mDraggedResources->resourcePaths[i], findUniquePath(newPath));
				}
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