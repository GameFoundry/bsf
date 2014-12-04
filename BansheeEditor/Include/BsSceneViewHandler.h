#pragma once

#include "BsEditorPrerequisites.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT SceneViewHandler
	{
	public:
		SceneViewHandler(const SPtr<CameraHandler>& camera);
		virtual ~SceneViewHandler();

		void update(const Vector2I& position);
		void pointerPressed(const Vector2I& position);
		void pointerReleased(const Vector2I& position, bool controlHeld);

	protected:
		void render(const Viewport* viewport, DrawList& drawList);

	private:
		SPtr<CameraHandler> mCamera;
		SceneGrid* mSceneGrid;
		HEvent mRenderCallback;
	};
}