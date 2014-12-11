#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT SceneViewHandler
	{
	public:
		SceneViewHandler(EditorWidgetBase* parentWidget, const SPtr<CameraHandler>& camera);
		virtual ~SceneViewHandler();

		void update(const Vector2I& position, const Vector2I& delta);
		void trySelectHandle(const Vector2I& position);
		bool isHandleActive() const;
		void clearHandleSelection();
		void pickObject(const Vector2I& position, bool additive);

	protected:
		void render(const Viewport* viewport, DrawList& drawList);

		void wrapCursorToWindow();

	private:
		EditorWidgetBase* mParentWidget;
		SPtr<CameraHandler> mCamera;
		SceneGrid* mSceneGrid;
		HEvent mRenderCallback;
	};
}