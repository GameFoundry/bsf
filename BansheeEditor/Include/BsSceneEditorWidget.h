#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorWidget.h"

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

	protected:
		void doOnResized(UINT32 width, UINT32 height);

		void updateRenderTexture(UINT32 width, UINT32 height);

	private:
		static SceneEditorWidget* Instance;

		RenderTexturePtr mSceneRenderTarget;
		GUIRenderTexture* mGUIRenderTexture;
		HCamera mCamera;
	};
}