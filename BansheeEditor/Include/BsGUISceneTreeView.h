#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUITreeView.h"
#include <boost/signal.hpp>

namespace BansheeEngine
{
	struct DraggedSceneObjects
	{
		DraggedSceneObjects(UINT32 numObjects);
		~DraggedSceneObjects();

		UINT32 numObjects;
		HSceneObject* objects;
	};

	class GUISceneTreeView : public GUITreeView
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


	protected:
		virtual ~GUISceneTreeView();

	protected:
		SceneTreeElement mRootElement;

		GUISceneTreeView(const String& backgroundStyle, const String& elementBtnStyle, 
			const String& foldoutBtnStyle, const String& selectionBackgroundStyle, const String& editBoxStyle, 
			const String& dragHighlightStyle, const String& dragSepHighlightStyle, const GUILayoutOptions& layoutOptions);

		void updateTreeElement(SceneTreeElement* element);

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

		void deleteTreeElementInternal(TreeElement* element);
	};
}