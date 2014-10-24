#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"
#include "BsServiceLocator.h"

namespace BansheeEngine
{
	class SceneEditorWidget : public EditorWidget<SceneEditorWidget>
	{
	public:
		SceneEditorWidget(const ConstructPrivately& dummy, EditorWidgetContainer& parentContainer);
		virtual ~SceneEditorWidget();

		virtual void _update();

		static SceneEditorWidget* instance();
		static SceneEditorWidget* open();
		static void close();
		static const String& getTypeName();

		const HCamera& getSceneCamera() const { return mCamera; }

	protected:
		void doOnResized(UINT32 width, UINT32 height);
		void doOnParentChanged();
		void determineParentWindow();

		void updateRenderTexture(UINT32 width, UINT32 height);
		void render(const Viewport* viewport, DrawList& drawList);
		bool toSceneViewPos(const Vector2I& screenPos, Vector2I& scenePos);

		/**
		 * @brief	Called whenever a pointer (e.g. mouse cursor) is moved.
		 */
		void onPointerMoved(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is released.
		 */
		void onPointerReleased(const PointerEvent& event);

		/**
		 * @brief	Called whenever a pointer button (e.g. mouse button) is pressed.
		 */
		void onPointerPressed(const PointerEvent& event);

	private:
		static SceneEditorWidget* Instance;

		RenderWindowPtr mParentWindow;
		RenderTexturePtr mSceneRenderTarget;
		GUIRenderTexture* mGUIRenderTexture;
		HCamera mCamera;
		GameObjectHandle<SceneCameraController> mCameraController;
		SceneGrid* mSceneGrid;

		bool mLeftButtonPressed;

		HEvent mRenderCallback;
		HEvent mOnPointerMovedConn;
		HEvent mOnPointerPressedConn;
		HEvent mOnPointerReleasedConn;
	};

	typedef ServiceLocator<SceneEditorWidget> SceneViewLocator;
}