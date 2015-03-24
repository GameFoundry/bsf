#pragma once

#include "BsEditorPrerequisites.h"
#include "BsVector2I.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT SceneViewHandler
	{
	public:
		SceneViewHandler(EditorWidgetBase* parentWidget, const SPtr<CameraHandler>& camera);
		virtual ~SceneViewHandler();

		void update();
		void updateHandle(const Vector2I& position, const Vector2I& delta);
		void trySelectHandle(const Vector2I& position);
		bool isHandleActive() const;
		void clearHandleSelection();
		void pickObject(const Vector2I& position, bool additive);

	protected:
		void render(const Viewport* viewport, DrawList& drawList);

		Vector2I wrapCursorToWindow();

	private:
		EditorWidgetBase* mParentWidget;
		SPtr<CameraHandler> mCamera;
		SceneGrid* mSceneGrid;
		SelectionRenderer* mSelectionRenderer;
		HEvent mRenderCallback;

		Vector2I mMouseDeltaCompensate;
	};
}