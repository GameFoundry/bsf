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

	class EditorApplication : public CM::Module<EditorApplication>
	{
	public:
		EditorApplication(RenderSystemPlugin renderSystemPlugin);
		~EditorApplication();

		void runMainLoop();

	private:
		RenderSystemPlugin mActiveRSPlugin;

		static const CM::String& getLibraryNameForRenderSystem(RenderSystemPlugin plugin);

		void update();
	};
}