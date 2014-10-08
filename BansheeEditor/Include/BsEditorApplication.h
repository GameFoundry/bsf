#pragma once

#include "BsEditorPrerequisites.h"
#include "BsApplication.h"
#include "BsPath.h"

namespace BansheeEngine
{
	class BS_ED_EXPORT EditorApplication : public Application
	{
	public:
		EditorApplication(RenderSystemPlugin renderSystemPlugin);
		virtual ~EditorApplication();

		static void startUp(RenderSystemPlugin renderSystemPlugin);

		bool isProjectLoaded() const;
		bool isGameViewFocused() const;
		bool isSceneViewFocused() const;
		const Path& getActiveProjectPath() const;


	private:
		virtual void onStartUp();
		virtual void update();

		EditorWidgetLayoutPtr loadWidgetLayout();
		void saveWidgetLayout(const EditorWidgetLayoutPtr& layout);

		static void closeModalWindow(RenderWindowPtr window, HSceneObject sceneObject);

	private:
		static const Path WIDGET_LAYOUT_PATH;
		RenderSystemPlugin mActiveRSPlugin;

		// DEBUG ONLY

		HGpuProgram mFragProgRef;
		HGpuProgram mVertProgRef;

		ShaderPtr mTestShader;
		TechniquePtr mNewTechniqueGL;
		PassPtr mNewPassGL;
		TechniquePtr mNewTechniqueDX;
		PassPtr mNewPassDX;
		TechniquePtr mNewTechniqueDX11;
		PassPtr mNewPassDX11;

		HMaterial mTestMaterial;
		HTexture mTestTexRef;
		HMesh mDbgMeshRef;
	};

	BS_ED_EXPORT EditorApplication& gEditorApplication();
}