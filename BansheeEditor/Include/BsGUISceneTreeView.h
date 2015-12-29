#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include "BsEvent.h"
#include "BsServiceLocator.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains SceneObject%s currently involved
	 *			in a drag and drop operation.
	 */
	struct BS_ED_EXPORT DraggedSceneObjects
	{
		DraggedSceneObjects(UINT32 numObjects);
		~DraggedSceneObjects();

		UINT32 numObjects;
		HSceneObject* objects;
	};

	/**
	 * @brief	GUI element that displays all SceneObject%s in the current scene
	 *			in the active project in a tree view.
	 */
	class BS_ED_EXPORT GUISceneTreeView : public GUITreeView
	{
		/**
		 * @brief	Tree element with SceneObject%-specific data.
		 */
		struct SceneTreeElement : public GUITreeView::TreeElement
		{
			SceneTreeElement()
				:mId(0), mIsPrefabInstance(false)
			{ }

			HSceneObject mSceneObject;
			UINT64 mId;
			bool mIsPrefabInstance;
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
		 * @param	highlightBackgroundStyle	Name of the style for the background of highlighted elements.
		 * @param	editBoxStyle				Name of the style for element that is being renamed.
		 * @param	dragHighlightStyle			Name of the style for the element being dragged over.
		 * @param	dragSepHighlightStyle		Name of the style for the separator displayed while dragging
		 *										an element between two other elements.
		 */	
		static GUISceneTreeView* create(
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& highlightBackgroundStyle = StringUtil::BLANK, 
			const String& selectionBackgroundStyle = StringUtil::BLANK, const String& editBoxStyle = StringUtil::BLANK, 
			const String& dragHighlightStyle = StringUtil::BLANK, const String& dragSepHighlightStyle = StringUtil::BLANK);

		/**
		 * @brief	Creates a new resource tree view element.
		 *
		 * @param	options						Options that allow you to control how is the element positioned and sized.
		 *										This will override any similar options set by style.
		 * @param	backgroundStyle				Name of the style for the tree view background.
		 * @param	elementBtnStyle				Name of the style for a normal tree view element.
		 * @param	foldoutBtnStyle				Name of the style for a foldout element (e.g. for a folder).
		 * @param	highlightBackgroundStyle	Name of the style for the background of highlighted elements.
		 * @param	selectionBackgroundStyle	Name of the style for the background of selected elements.
		 * @param	editBoxStyle				Name of the style for element that is being renamed.
		 * @param	dragHighlightStyle			Name of the style for the element being dragged over.
		 * @param	dragSepHighlightStyle		Name of the style for the separator displayed while dragging
		 *										an element between two other elements.
		 */	
		static GUISceneTreeView* create(const GUIOptions& options, 
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& highlightBackgroundStyle = StringUtil::BLANK,
			const String& selectionBackgroundStyle = StringUtil::BLANK, const String& editBoxStyle = StringUtil::BLANK, 
			const String& dragHighlightStyle = StringUtil::BLANK, const String& dragSepHighlightStyle = StringUtil::BLANK);

		/**
		 * @brief	Returns a list of SceneObject&s currently selected (if any).
		 */	
		Vector<HSceneObject> getSelection() const;

		/**
		 * @brief	Changes the active selection to the provided SceneObject%s.
		 */	
		void setSelection(const Vector<HSceneObject>& objects);

		/**
		 * @brief	Scrolls to and highlights the selected object (does not select it).
		 */
		void ping(const HSceneObject& object);

		/** Triggered whenever the selection changes. Call getSelection() to retrieve new selection. */
		Event<void()> onSelectionChanged; 

		/** 
		 * Triggered whenever the scene is modified in any way from within the scene tree view (e.g. object is deleted, 
		 * added, etc.).
		 */
		Event<void()> onModified; 

		/**
		 * Triggered when a resource drag and drop operation finishes over the scene tree view. Provided scene object
		 * is the tree view element that the operation finished over (or null if none), and the list of paths is the list 
		 * of relative paths of the resources that were dropped.
		 */
		Event<void(const HSceneObject&, const Vector<Path>&)> onResourceDropped;
		
		static const MessageId SELECTION_CHANGED_MSG;
	protected:
		virtual ~GUISceneTreeView();

	protected:
		SceneTreeElement mRootElement;

		GUISceneTreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& highlightBackgroundStyle, const String& selectionBackgroundStyle, 
			const String& editBoxStyle, const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUIDimensions& dimensions);

		/**
		 * @brief	Checks it the SceneObject referenced by this tree element changed in any way and updates
		 *			the tree element. This can involve recursing all children and updating them as well.
		 */
		void updateTreeElement(SceneTreeElement* element);

		/**
		 * @brief	Triggered when a drag and drop operation that was started by the tree view
		 *			ends, regardless if it was processed or not.
		 */
		void dragAndDropFinalize();

		/**
		 * @copydoc	TreeView::getRootElement
		 */
		virtual TreeElement& getRootElement() override { return mRootElement; }

		/**
		 * @copydoc	TreeView::getRootElementConst
		 */
		virtual const TreeElement& getRootElementConst() const override { return mRootElement; }

		/**
		 * @copydoc	TreeView::updateTreeElementHierarchy
		 */
		virtual void updateTreeElementHierarchy() override;

		/**
		 * @copydoc	TreeView::renameTreeElement
		 */
		virtual void renameTreeElement(TreeElement* element, const WString& name) override;

		/**
		 * @copydoc	TreeView::deleteTreeElement
		 */
		virtual void deleteTreeElement(TreeElement* element) override;

		/**
		 * @copydoc	TreeView::acceptDragAndDrop
		 */
		virtual bool acceptDragAndDrop() const override;

		/**
		 * @copydoc	TreeView::dragAndDropStart
		 */
		virtual void dragAndDropStart() override;

		/**
		 * @copydoc	TreeView::dragAndDropEnded
		 */
		virtual void dragAndDropEnded(TreeElement* overTreeElement) override;

		/**
		 * @copydoc	TreeView::_acceptDragAndDrop
		 */
		virtual bool _acceptDragAndDrop(const Vector2I position, UINT32 typeId) const override;

		/**
		 * @copydoc	TreeView::selectionChanged
		 */
		virtual void selectionChanged() override;

		/**
		 * @brief	Deletes the internal TreeElement representation without
		 *			actually deleting the referenced SceneObject.
		 */
		void deleteTreeElementInternal(TreeElement* element);

		/**
		 * @brief	Attempts to find a tree element referencing the specified
		 *			scene object.
		 */
		SceneTreeElement* findTreeElement(const HSceneObject& so);

		/**
		 * @copydoc	GUITreeView::duplicateSelection
		 */
		void duplicateSelection() override;

		/**
		 * @copydoc	GUITreeView::copySelection
		 */
		void copySelection() override;
		
		/**
		 * @copydoc	GUITreeView::cutSelection
		 */
		void cutSelection() override;

		/**
		 * @copydoc	GUITreeView::paste
		 */
		void paste() override;

		/**
		 * @brief	Creates a new scene object as a child of the currently selected object (if any).
		 */
		void createNewSO();

		/**
		 * @brief	Removes all elements from the list used for copy/cut operations.
		 */
		void clearCopyList();

		/**
		 * @brief	Cleans duplicate objects from the provided scene object list.
		 *			This involves removing child elements if their parents are
		 *			already part of the list.
		 */
		static void cleanDuplicates(Vector<HSceneObject>& objects);

		Vector<HSceneObject> mCopyList;
		bool mCutFlag;

		static const Color PREFAB_TINT;
	};

	typedef ServiceLocator<GUISceneTreeView> SceneTreeViewLocator;
}