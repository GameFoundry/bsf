#pragma once

#include "BsEditorPrerequisites.h"
#include "BsServiceLocator.h"
#include "BsGUITreeView.h"
#include "BsProjectLibrary.h"
#include "BsEvent.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains paths to resources currently involved
	 *			in a drag and drop operation.
	 */
	struct DraggedResources
	{
		Vector<Path> resourcePaths;
	};

	/**
	 * @brief	GUI element that displays all resources in the active project in a tree view.
	 */
	class GUIResourceTreeView : public GUITreeView
	{
		/**
		 * @brief	Tree element with resource-specific data.
		 */
		struct ResourceTreeElement : public GUITreeView::TreeElement
		{
			Path mFullPath;
			WString mElementName;
		};

		/**
		 * @brief	Contains paths to resources currently involved
		 *			in a drag and drop operation initiated by this tree view.
		 */
		struct InternalDraggedResources
		{
			InternalDraggedResources(UINT32 numObjects);
			~InternalDraggedResources();

			UINT32 numObjects;
			Path* resourcePaths;
		};

	public:
		/**
		 * Returns type name of the GUI element used for finding GUI element styles. 
		 */
		static const String& getGUITypeName();

		/**
		 * @brief	Creates a new resource tree view element.
		 *
		 * @param	backgroundStyle				Name of the style for the tree view background.
		 * @param	elementBtnStyle				Name of the style for a normal tree view element.
		 * @param	foldoutBtnStyle				Name of the style for a foldout element (e.g. for a folder).
		 * @param	selectionBackgroundStyle	Name of the style for the background of selected elements.
		 * @param	editBoxStyle				Name of the style for element that is being renamed.
		 * @param	dragHighlightStyle			Name of the style for the element being dragged over.
		 * @param	dragSepHighlightStyle		Name of the style for the separator displayed while dragging
		 *										an element between two other elements.
		 */	
		static GUIResourceTreeView* create(
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& selectionBackgroundStyle = StringUtil::BLANK,
			const String& editBoxStyle = StringUtil::BLANK, const String& dragHighlightStyle = StringUtil::BLANK, 
			const String& dragSepHighlightStyle = StringUtil::BLANK);

		/**
		 * @brief	Creates a new resource tree view element.
		 *
		 * @param	options						Options that allow you to control how is the element positioned and sized.
		 *										This will override any similar options set by style.
		 * @param	backgroundStyle				Name of the style for the tree view background.
		 * @param	elementBtnStyle				Name of the style for a normal tree view element.
		 * @param	foldoutBtnStyle				Name of the style for a foldout element (e.g. for a folder).
		 * @param	selectionBackgroundStyle	Name of the style for the background of selected elements.
		 * @param	editBoxStyle				Name of the style for element that is being renamed.
		 * @param	dragHighlightStyle			Name of the style for the element being dragged over.
		 * @param	dragSepHighlightStyle		Name of the style for the separator displayed while dragging
		 *										an element between two other elements.
		 */	
		static GUIResourceTreeView* create(const GUIOptions& options, 
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& selectionBackgroundStyle = StringUtil::BLANK,
			const String& editBoxStyle = StringUtil::BLANK, const String& dragHighlightStyle = StringUtil::BLANK, 
			const String& dragSepHighlightStyle = StringUtil::BLANK);

		/**
		 * @brief	Returns a list of paths of currently selected resources (if any).
		 *			Returned paths are relative to the project folder.
		 */	
		Vector<Path> getSelection() const;

		/**
		 * @brief	Changes the active selection to the provided resources.
		 *			Paths can be absolute or relative.
		 */	
		void setSelection(const Vector<Path>& paths);

		Event<void()> onSelectionChanged; /**< Triggered whenever the selection changes. Call ::getSelection() to retrieve new selection: */
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

		/**
		 * @copydoc	GUITreeView::_updateLayoutInternal
		 */	
		virtual void _updateLayoutInternal(const GUILayoutData& data) override;

		/**
		 * @copydoc	GUITreeView::getRootElement
		 */	
		virtual TreeElement& getRootElement() override { return mRootElement; }

		/**
		 * @copydoc	GUITreeView::getRootElementConst
		 */	
		virtual const TreeElement& getRootElementConst() const override { return mRootElement; }

		/**
		 * @copydoc	GUITreeView::updateTreeElementHierarchy
		 */	
		virtual void updateTreeElementHierarchy() override;

		/**
		 * @copydoc	GUITreeView::renameTreeElement
		 */	
		virtual void renameTreeElement(TreeElement* element, const WString& name) override;

		/**
		 * @copydoc	GUITreeView::deleteTreeElement
		 */	
		virtual void deleteTreeElement(TreeElement* element) override;

		/**
		 * @copydoc	GUITreeView::acceptDragAndDrop
		 */	
		virtual bool acceptDragAndDrop() const override;

		/**
		 * @copydoc	GUITreeView::dragAndDropStart
		 */	
		virtual void dragAndDropStart() override;

		/**
		 * @copydoc	GUITreeView::dragAndDropEnded
		 */	
		virtual void dragAndDropEnded(TreeElement* overTreeElement) override;

		/**
		 * @copydoc	GUITreeView::dragAndDropFinalize
		 */	
		virtual void dragAndDropFinalize() override;

		/**
		 * @copydoc	GUITreeView::_acceptDragAndDrop
		 */	
		virtual bool _acceptDragAndDrop(const Vector2I position, UINT32 typeId) const override;

		/**
		 * @copydoc	GUITreeView::selectionChanged
		 */	
		virtual void selectionChanged() override;

		/**
		 * @copydoc	GUITreeView::_changeParentWidget
		 */	
		void _changeParentWidget(GUIWidget* widget) override;

		/**
		 * @brief	Updates the contents of the provided tree entry with a project library entry.
		 */
		void updateFromProjectLibraryEntry(ResourceTreeElement* treeElement, const ProjectLibrary::LibraryEntry* libraryEntry);

		/**
		 * @brief	Creates a new tree view entry.
		 *
		 * @param	parent		Parent tree view entry to create the new one for.
		 * @param	fullPath	Absolute path to the new tree entry.
		 */
		ResourceTreeElement* addTreeElement(ResourceTreeElement* parent, const Path& fullPath);

		/**
		 * @brief	Deletes the provided tree element.
		 */
		void deleteTreeElement(ResourceTreeElement* element);

		/**
		 * @brief	Sorts the children of the provided tree element by name.
		 */
		void sortTreeElement(ResourceTreeElement* element);

		/**
		 * @brief	Attempts to find a tree element with the specified path.
		 *			Returns null if one cannot be found.
		 */
		ResourceTreeElement* findTreeElement(const Path& fullPath);

		/**
		 * @brief	Called whenever a new entry is added to the project library.
		 */
		void entryAdded(const Path& path); 

		/**
		 * @brief	Called whenever an entry is removed from the project library.
		 */
		void entryRemoved(const Path& path);

		/**
		 * @brief	Sets an OS drag and drop target that allows this element to receive OS-specific
		 *			drag and drop events originating from other processes.
		 */
		void setDropTarget(RenderWindow* parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);

		/**
		 * @brief	Removes the currently set OS drop target.
		 */
		void clearDropTarget();

		/**
		 * @brief	Triggered whenever the user drags the pointer over the set drop target,
		 *			while OS drag and drop operation is active.
		 *
		 * @param	x	X coordinate of the pointer, relative to drop area.
		 * @param	y	Y coordinate of the pointer, relative to drop area.
		 */
		void dropTargetDragMove(INT32 x, INT32 y);

		/**
		 * @brief	Triggered whenever pointer leaves the drop target,
		 *			while OS drag and drop operation is active.
		 */
		void dropTargetDragLeave();

		/**
		 * @brief	Triggered whenever the user releases the pointer over the set drop target,
		 *			while OS drag and drop operation is active.
		 *
		 * @param	x	X coordinate of the pointer, relative to drop area.
		 * @param	y	Y coordinate of the pointer, relative to drop area.
		 */
		void dropTargetDragDropped(INT32 x, INT32 y);

		/**
		 * @brief	Generates a unique path in the case that something already
		 *			exists on the provided one.
		 */
		Path findUniquePath(const Path& path);
	};

	typedef ServiceLocator<GUIResourceTreeView> ResourceTreeViewLocator;
}