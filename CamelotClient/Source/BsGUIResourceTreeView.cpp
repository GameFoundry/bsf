#include "BsGUIResourceTreeView.h"
#include "BsGUISkin.h"
#include "BsProjectLibrary.h"
#include "BsDragAndDropManager.h"
#include "CmResources.h"
#include "CmResourceManifest.h"
#include "BsProjectLibrary.h"
#include "CmFileSystem.h"

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
		dragSepHighlightStyle, layoutOptions), mDraggedResources(nullptr)
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
	}

	GUIResourceTreeView::~GUIResourceTreeView()
	{

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
		}
		else
			return path;
	}

	bool GUIResourceTreeView::acceptDragAndDrop() const
	{
		return DragAndDropManager::instance().isDragInProgress() && DragAndDropManager::instance().getDragTypeId() == (UINT32)DragAndDropType::Resources;
	}

	void GUIResourceTreeView::dragAndDropStart()
	{
		assert(mDraggedResources == nullptr);

		DraggedResources* draggedResources = cm_new<DraggedResources>();
		InternalDraggedResources* internalDraggedResources = cm_new<InternalDraggedResources>((UINT32)mSelectedElements.size());

		ResourceManifestPtr resourceManifest = gResources().getResourceManifest();

		UINT32 cnt = 0;
		UINT32 numValidResources = 0;
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

	const String& GUIResourceTreeView::getGUITypeName()
	{
		static String typeName = "ResourceTreeView";
		return typeName;
	}
}