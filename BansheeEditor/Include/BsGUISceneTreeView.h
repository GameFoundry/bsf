#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
	struct DraggedSceneObjects
	{
		DraggedSceneObjects(CM::UINT32 numObjects);
		~DraggedSceneObjects();

		CM::UINT32 numObjects;
		CM::HSceneObject* objects;
	};

	class GUISceneTreeView : public GUITreeView
	{
		struct SceneTreeElement : public GUITreeView::TreeElement
		{
			SceneTreeElement()
				:mId(0)
			{ }

			CM::HSceneObject mSceneObject;
			CM::UINT64 mId;
		};

	public:
		static const CM::String& getGUITypeName();

		static GUISceneTreeView* create(
			const CM::String& backgroundStyle = CM::StringUtil::BLANK, const CM::String& elementBtnStyle = CM::StringUtil::BLANK, 
			const CM::String& foldoutBtnStyle = CM::StringUtil::BLANK, const CM::String& selectionBackgroundStyle = CM::StringUtil::BLANK,
			const CM::String& editBoxStyle = CM::StringUtil::BLANK, const CM::String& dragHighlightStyle = CM::StringUtil::BLANK, 
			const CM::String& dragSepHighlightStyle = CM::StringUtil::BLANK);

		static GUISceneTreeView* create(const BS::GUIOptions& options, 
			const CM::String& backgroundStyle = CM::StringUtil::BLANK, const CM::String& elementBtnStyle = CM::StringUtil::BLANK, 
			const CM::String& foldoutBtnStyle = CM::StringUtil::BLANK, const CM::String& selectionBackgroundStyle = CM::StringUtil::BLANK,
			const CM::String& editBoxStyle = CM::StringUtil::BLANK, const CM::String& dragHighlightStyle = CM::StringUtil::BLANK, 
			const CM::String& dragSepHighlightStyle = CM::StringUtil::BLANK);


	protected:
		virtual ~GUISceneTreeView();

	protected:
		SceneTreeElement mRootElement;

		GUISceneTreeView(const CM::String& backgroundStyle, const CM::String& elementBtnStyle, 
			const CM::String& foldoutBtnStyle, const CM::String& selectionBackgroundStyle, const CM::String& editBoxStyle, 
			const CM::String& dragHighlightStyle, const CM::String& dragSepHighlightStyle, const BS::GUILayoutOptions& layoutOptions);

		void updateTreeElement(SceneTreeElement* element);

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

		void deleteTreeElementInternal(TreeElement* element);
	};
}