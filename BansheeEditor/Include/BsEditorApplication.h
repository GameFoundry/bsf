#pragma once

#include "BsEditorPrerequisites.h"
#include "BsApplication.h"

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
		const Path& getProjectPath() const;
		const WString& getProjectName() const;

		/**
		 * @brief	Returns the absolute path to the built-in managed editor assembly file.
		 */
		Path getEditorAssemblyPath() const;

		/**
		 * @brief	Returns the absolute path of the managed editor script assembly file.
		 */
		Path getEditorScriptAssemblyPath() const;

		/**
		 * @copydoc	Application::getGameAssemblyPath
		 */
		Path getGameAssemblyPath() const override;

		EditorSettingsPtr getEditorSettings() const { return mEditorSettings; }

	private:
		virtual void onStartUp();
		virtual void onShutDown();
		virtual void update();

		EditorWidgetLayoutPtr loadWidgetLayout();
		void saveWidgetLayout(const EditorWidgetLayoutPtr& layout);

		static void closeModalWindow(RenderWindowPtr window, HSceneObject sceneObject);

	private:
		static const Path WIDGET_LAYOUT_PATH;
		RenderSystemPlugin mActiveRSPlugin;
		EditorSettingsPtr mEditorSettings;

		DynLib* mSBansheeEditorPlugin;

		// DEBUG ONLY

		HGpuProgram mFragProgRef;
		HGpuProgram mVertProgRef;

		HShader mTestShader;
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