#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include "BsProjectLibrary.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	class GUIResourceTreeView : public GUITreeView
	{
		struct ResourceTreeElement : public GUITreeView::TreeElement
		{
			CM::WString mFullPath;
			CM::WString mElementName;
		};

		struct DraggedResources
		{
			CM::Vector<CM::String>::type resourceUUIDs;
		};

		struct InternalDraggedResources
		{
			InternalDraggedResources(CM::UINT32 numObjects);
			~InternalDraggedResources();

			CM::UINT32 numObjects;
			CM::WString* resourcePaths;
		};

	public:
		static const CM::String& getGUITypeName();

		static GUIResourceTreeView* create(
			const CM::String& backgroundStyle = CM::StringUtil::BLANK, const CM::String& elementBtnStyle = CM::StringUtil::BLANK, 
			const CM::String& foldoutBtnStyle = CM::StringUtil::BLANK, const CM::String& selectionBackgroundStyle = CM::StringUtil::BLANK,
			const CM::String& editBoxStyle = CM::StringUtil::BLANK, const CM::String& dragHighlightStyle = CM::StringUtil::BLANK, 
			const CM::String& dragSepHighlightStyle = CM::StringUtil::BLANK);

		static GUIResourceTreeView* create(const BS::GUIOptions& options, 
			const CM::String& backgroundStyle = CM::StringUtil::BLANK, const CM::String& elementBtnStyle = CM::StringUtil::BLANK, 
			const CM::String& foldoutBtnStyle = CM::StringUtil::BLANK, const CM::String& selectionBackgroundStyle = CM::StringUtil::BLANK,
			const CM::String& editBoxStyle = CM::StringUtil::BLANK, const CM::String& dragHighlightStyle = CM::StringUtil::BLANK, 
			const CM::String& dragSepHighlightStyle = CM::StringUtil::BLANK);


	protected:
		virtual ~GUIResourceTreeView();

	protected:
		InternalDraggedResources* mDraggedResources;
		ResourceTreeElement mRootElement;
		CM::RenderWindow* mCurrentWindow;
		CM::OSDropTarget* mDropTarget;
		bool mDropTargetDragActive;

		boost::signals::connection mDropTargetEnterConn;
		boost::signals::connection mDropTargetMoveConn;
		boost::signals::connection mDropTargetLeaveConn;
		boost::signals::connection mDropTargetDroppedConn;

		GUIResourceTreeView(const CM::String& backgroundStyle, const CM::String& elementBtnStyle, 
			const CM::String& foldoutBtnStyle, const CM::String& selectionBackgroundStyle, const CM::String& editBoxStyle, 
			const CM::String& dragHighlightStyle, const CM::String& dragSepHighlightStyle, const BS::GUILayoutOptions& layoutOptions);

		virtual void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		virtual TreeElement& getRootElement() { return mRootElement; }
		virtual const TreeElement& getRootElementConst() const { return mRootElement; }
		virtual void updateTreeElementHierarchy();
		virtual void renameTreeElement(TreeElement* element, const CM::WString& name);
		virtual void deleteTreeElement(TreeElement* element);
		virtual bool acceptDragAndDrop() const;
		virtual void dragAndDropStart();
		virtual void dragAndDropEnded(TreeElement* overTreeElement);
		virtual void dragAndDropFinalize();
		virtual bool _acceptDragAndDrop(const CM::Vector2I position, CM::UINT32 typeId) const;

		void updateFromProjectLibraryEntry(ResourceTreeElement* treeElement, const ProjectLibrary::LibraryEntry* libraryEntry);
		ResourceTreeElement* addTreeElement(ResourceTreeElement* parent, const CM::WString& fullPath);
		void deleteTreeElement(ResourceTreeElement* element);
		void sortTreeElement(ResourceTreeElement* element);

		ResourceTreeElement* findTreeElement(const CM::WString& fullPath);

		void entryAdded(const CM::WString& path);
		void entryRemoved(const CM::WString& path);

		void setDropTarget(CM::RenderWindow* parentWindow, CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height);
		void clearDropTarget();

		void dropTargetDragMove(CM::INT32 x, CM::INT32 y);
		void dropTargetDragLeave();
		void dropTargetDragDropped(CM::INT32 x, CM::INT32 y);

		CM::WString findUniquePath(const CM::WString& path);

		void _changeParentWidget(BS::GUIWidget* widget);
	};
}