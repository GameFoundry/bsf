#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include "BsProjectLibrary.h"
#include <boost/signal.hpp>

namespace BansheeEngine
{
	class GUIResourceTreeView : public GUITreeView
	{
		struct ResourceTreeElement : public GUITreeView::TreeElement
		{
			WString mFullPath;
			WString mElementName;
		};

		struct DraggedResources
		{
			Vector<String>::type resourceUUIDs;
		};

		struct InternalDraggedResources
		{
			InternalDraggedResources(UINT32 numObjects);
			~InternalDraggedResources();

			UINT32 numObjects;
			WString* resourcePaths;
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


	protected:
		virtual ~GUIResourceTreeView();

	protected:
		InternalDraggedResources* mDraggedResources;
		ResourceTreeElement mRootElement;
		RenderWindow* mCurrentWindow;
		OSDropTarget* mDropTarget;
		bool mDropTargetDragActive;

		boost::signals::connection mDropTargetEnterConn;
		boost::signals::connection mDropTargetMoveConn;
		boost::signals::connection mDropTargetLeaveConn;
		boost::signals::connection mDropTargetDroppedConn;

		GUIResourceTreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, 
			const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUILayoutOptions& layoutOptions);

		virtual void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

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

		void updateFromProjectLibraryEntry(ResourceTreeElement* treeElement, const ProjectLibrary::LibraryEntry* libraryEntry);
		ResourceTreeElement* addTreeElement(ResourceTreeElement* parent, const WString& fullPath);
		void deleteTreeElement(ResourceTreeElement* element);
		void sortTreeElement(ResourceTreeElement* element);

		ResourceTreeElement* findTreeElement(const WString& fullPath);

		void entryAdded(const WString& path);
		void entryRemoved(const WString& path);

		void setDropTarget(RenderWindow* parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		void clearDropTarget();

		void dropTargetDragMove(INT32 x, INT32 y);
		void dropTargetDragLeave();
		void dropTargetDragDropped(INT32 x, INT32 y);

		WString findUniquePath(const WString& path);

		void _changeParentWidget(GUIWidget* widget);
	};
}