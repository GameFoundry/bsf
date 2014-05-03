#pragma once

#include "BsEditorPrerequisites.h"
#include "CmModule.h"
#include "CmPath.h"

namespace BansheeEngine
{
	enum class RenderSystemPlugin
	{
		DX11,
		DX9,
		OpenGL
	};

	class BS_ED_EXPORT EditorApplication : public Module<EditorApplication>
	{
	public:
		EditorApplication(RenderSystemPlugin renderSystemPlugin);
		~EditorApplication();

		void runMainLoop();

		bool isProjectLoaded() const;
		const Path& getActiveProjectPath() const;
	private:
		static const Path WIDGET_LAYOUT_PATH;
		RenderSystemPlugin mActiveRSPlugin;

		static const String& getLibraryNameForRenderSystem(RenderSystemPlugin plugin);

		void update();

		EditorWidgetLayoutPtr loadWidgetLayout();
		void saveWidgetLayout(const EditorWidgetLayoutPtr& layout);

		static void closeModalWindow(RenderWindowPtr window, HSceneObject sceneObject);
	};
}