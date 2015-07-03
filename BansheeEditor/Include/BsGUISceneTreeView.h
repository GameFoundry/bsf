#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include "BsEvent.h"
#include "BsServiceLocator.h"

namespace BansheeEngine
{
	struct BS_ED_EXPORT DraggedSceneObjects
	{
		DraggedSceneObjects(UINT32 numObjects);
		~DraggedSceneObjects();

		UINT32 numObjects;
		HSceneObject* objects;
	};

	class BS_ED_EXPORT GUISceneTreeView : public GUITreeView
	{
		struct SceneTreeElement : public GUITreeView::TreeElement
		{
			SceneTreeElement()
				:mId(0)
			{ }

			HSceneObject mSceneObject;
			UINT64 mId;
		};

	public:
		static const String& getGUITypeName();

		static GUISceneTreeView* create(
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& selectionBackgroundStyle = StringUtil::BLANK,
			const String& editBoxStyle = StringUtil::BLANK, const String& dragHighlightStyle = StringUtil::BLANK, 
			const String& dragSepHighlightStyle = StringUtil::BLANK);

		static GUISceneTreeView* create(const GUIOptions& options, 
			const String& backgroundStyle = StringUtil::BLANK, const String& elementBtnStyle = StringUtil::BLANK, 
			const String& foldoutBtnStyle = StringUtil::BLANK, const String& selectionBackgroundStyle = StringUtil::BLANK,
			const String& editBoxStyle = StringUtil::BLANK, const String& dragHighlightStyle = StringUtil::BLANK, 
			const String& dragSepHighlightStyle = StringUtil::BLANK);

		Vector<HSceneObject> getSelection() const;
		void setSelection(const Vector<HSceneObject>& objects);

		Event<void()> onSelectionChanged;
		static const MessageId SELECTION_CHANGED_MSG;
	protected:
		virtual ~GUISceneTreeView();

	protected:
		SceneTreeElement mRootElement;

		GUISceneTreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, 
			const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUIDimensions& dimensions);

		void updateTreeElement(SceneTreeElement* element);

		virtual TreeElement& getRootElement() override { return mRootElement; }
		virtual const TreeElement& getRootElementConst() const override { return mRootElement; }
		virtual void updateTreeElementHierarchy() override;
		virtual void renameTreeElement(TreeElement* element, const WString& name) override;
		virtual void deleteTreeElement(TreeElement* element) override;
		virtual bool acceptDragAndDrop() const override;
		virtual void dragAndDropStart() override;
		virtual void dragAndDropEnded(TreeElement* overTreeElement) override;
		virtual void dragAndDropFinalize() override;
		virtual bool _acceptDragAndDrop(const Vector2I position, UINT32 typeId) const override;
		virtual void selectionChanged() override;

		void deleteTreeElementInternal(TreeElement* element);
	};

	typedef ServiceLocator<GUISceneTreeView> SceneTreeViewLocator;
}