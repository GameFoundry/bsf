#pragma once

#include "BsEditorPrerequisites.h"
#include "BsServiceLocator.h"
#include "BsGUITreeView.h"
#include "BsProjectLibrary.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	struct DraggedResources
	{
		Vector<Path> resourcePaths;
	};

	class GUIResourceTreeView : public GUITreeView
	{
		struct ResourceTreeElement : public GUITreeView::TreeElement
		{
			Path mFullPath;
			WString mElementName;
		};

		struct InternalDraggedResources
		{
			InternalDraggedResources(UINT32 numObjects);
			~InternalDraggedResources();

			UINT32 numObjects;
			Path* resourcePaths;
		};

	public:
		static const String& getGUITypeName();

		static GUIResourceTreeView* create(
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& selectionBackgroundStyle = StringUtil::BLANK,
			const String& editBoxStyle = StringUtil::BLANK, const String& dragHighlightStyle = StringUtil::BLANK, 
			const String& dragSepHighlightStyle = StringUtil::BLANK);

		static GUIResourceTreeView* create(const GUIOptions& options, 
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& selectionBackgroundStyle = StringUtil::BLANK,
			const String& editBoxStyle = StringUtil::BLANK, const String& dragHighlightStyle = StringUtil::BLANK, 
			const String& dragSepHighlightStyle = StringUtil::BLANK);

		Vector<Path> getSelection() const;
		void setSelection(const Vector<Path>& paths);

		Event<void()> onSelectionChanged;
		static const MessageId SELECTION_CHANGED_MSG;

	protected:
		virtual ~GUIResourceTreeView();

	protected:
		InternalDraggedResources* mDraggedResources;
		ResourceTreeElement mRootElement;
		RenderWindow* mCurrentWindow;
		OSDropTarget* mDropTarget;
		bool mDropTargetDragActive;

		HEvent mDropTargetEnterConn;
		HEvent mDropTargetMoveConn;
		HEvent mDropTargetLeaveConn;
		HEvent mDropTargetDroppedConn;

		GUIResourceTreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, 
			const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUIDimensions& dimensions);

		virtual void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			Rect2I clipRect, UINT8 widgetDepth, INT16 panelDepth, UINT16 panelDepthRangeMin, UINT16 panelDepthRangeMax);

		virtual TreeElement& getRootElement() { return mRootElement; }
		virtual const TreeElement& getRootElementConst() const { return mRootElement; }
		virtual void updateTreeElementHierarchy();
		virtual void renameTreeElement(TreeElement* element, const WString& name);
		virtual void deleteTreeElement(TreeElement* element);
		virtual bool acceptDragAndDrop() const;
		virtual void dragAndDropStart();
		virtual void dragAndDropEnded(TreeElement* overTreeElement);
		virtual void dragAndDropFinalize();
		virtual bool _acceptDragAndDrop(const Vector2I position, UINT32 typeId) const;
		virtual void selectionChanged();

		void updateFromProjectLibraryEntry(ResourceTreeElement* treeElement, const ProjectLibrary::LibraryEntry* libraryEntry);
		ResourceTreeElement* addTreeElement(ResourceTreeElement* parent, const Path& fullPath);
		void deleteTreeElement(ResourceTreeElement* element);
		void sortTreeElement(ResourceTreeElement* element);

		ResourceTreeElement* findTreeElement(const Path& fullPath);

		void entryAdded(const Path& path);
		void entryRemoved(const Path& path);

		void setDropTarget(RenderWindow* parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		void clearDropTarget();

		void dropTargetDragMove(INT32 x, INT32 y);
		void dropTargetDragLeave();
		void dropTargetDragDropped(INT32 x, INT32 y);

		Path findUniquePath(const Path& path);

		void _changeParentWidget(GUIWidget* widget);
	};

	typedef ServiceLocator<GUIResourceTreeView> ResourceTreeViewLocator;
}