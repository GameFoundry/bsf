#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include <boost/signal.hpp>

namespace BansheeEditor
{
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

		struct DraggedSceneObjects
		{
			DraggedSceneObjects(CM::UINT32 numObjects);
			~DraggedSceneObjects();

			CM::UINT32 numObjects;
			CM::HSceneObject* objects;
		};

	public:
		static const CM::String& getGUITypeName();

		static GUISceneTreeView* create(BS::GUIWidget& parent,
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr, BS::GUIElementStyle* selectionBackgroundStyle = nullptr,
			BS::GUIElementStyle* editBoxStyle = nullptr, BS::GUIElementStyle* dragHighlightStyle = nullptr, 
			BS::GUIElementStyle* dragSepHighlightStyle = nullptr);

		static GUISceneTreeView* create(BS::GUIWidget& parent, const BS::GUIOptions& options, 
			BS::GUIElementStyle* backgroundStyle = nullptr, BS::GUIElementStyle* elementBtnStyle = nullptr, 
			BS::GUIElementStyle* foldoutBtnStyle = nullptr, BS::GUIElementStyle* selectionBackgroundStyle = nullptr,
			BS::GUIElementStyle* editBoxStyle = nullptr, BS::GUIElementStyle* dragHighlightStyle = nullptr, 
			BS::GUIElementStyle* dragSepHighlightStyle = nullptr);


	protected:
		virtual ~GUISceneTreeView();

	protected:
		SceneTreeElement mRootElement;

		GUISceneTreeView(BS::GUIWidget& parent, BS::GUIElementStyle* backgroundStyle, BS::GUIElementStyle* elementBtnStyle, 
			BS::GUIElementStyle* foldoutBtnStyle, BS::GUIElementStyle* selectionBackgroundStyle, BS::GUIElementStyle* editBoxStyle, 
			BS::GUIElementStyle* dragHighlightStyle, BS::GUIElementStyle* dragSepHighlightStyle, const BS::GUILayoutOptions& layoutOptions);

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

		void deleteTreeElementInternal(TreeElement* element);
	};
}