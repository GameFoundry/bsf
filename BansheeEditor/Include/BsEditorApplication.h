#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"

namespace BansheeEditor
{
	enum class RenderSystemPlugin
	{
		DX11,
		DX9,
		OpenGL
	};

	class BS_ED_EXPORT EditorApplication : public CM::Module<EditorApplication>
	{
	public:
		EditorApplication(RenderSystemPlugin renderSystemPlugin);
		~EditorApplication();

		void runMainLoop();

		bool isProjectLoaded() const;
		const CM::WString& getActiveProjectPath() const;
	private:
		static const CM::WString WIDGET_LAYOUT_PATH;
		RenderSystemPlugin mActiveRSPlugin;

		static const CM::String& getLibraryNameForRenderSystem(RenderSystemPlugin plugin);

		void update();

		EditorWidgetLayoutPtr loadWidgetLayout();
		void saveWidgetLayout(const EditorWidgetLayoutPtr& layout);

		static void closeModalWindow(CM::RenderWindowPtr window, CM::HSceneObject sceneObject);
	};
}