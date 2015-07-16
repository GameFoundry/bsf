#pragma once

#include "BsEditorPrerequisites.h"
#include "BsApplication.h"

namespace BansheeEngine
{
	/**
	 * @brief	Primary editor class containing the editor entry point.
	 */
	class BS_ED_EXPORT EditorApplication : public Application
	{
	public:
		EditorApplication(RenderSystemPlugin renderSystemPlugin);
		virtual ~EditorApplication();

		/**
		 * @brief	Starts the editorn with the specified render system.
		 */
		static void startUp(RenderSystemPlugin renderSystemPlugin);

		/**
		 * @brief	Checks whether the editor currently has a project loaded.
		 */
		bool isProjectLoaded() const;

		/**
		 * @brief	Returns the path to the currently loaded project.
		 */
		const Path& getProjectPath() const;

		/**
		 * @brief	Returns the name of the currently loaded project.
		 */
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
		 * @copydoc	Application::getScriptAssemblyFolder
		 */
		Path getScriptAssemblyFolder() const override;

		/**
		 * @brief	Returns a set of serializable editor settings that contain
		 *			every globally customizable editor property.
		 */
		EditorSettingsPtr getEditorSettings() const { return mEditorSettings; }

	private:
		virtual void onStartUp() override;
		virtual void onShutDown() override;
		virtual void preUpdate() override;
		virtual void postUpdate() override;

		/**
		 * @brief	Loads the previously saved editor widget layout from the default location.
		 *			Can return null if no layout was previously saved.
		 */
		EditorWidgetLayoutPtr loadWidgetLayout();

		/**
		 * @brief	Saves the provided widget layout at the default layout location.
		 */
		void saveWidgetLayout(const EditorWidgetLayoutPtr& layout);

		/**
		 * @copydoc	Application::getShaderIncludeHandler
		 */
		virtual ShaderIncludeHandlerPtr getShaderIncludeHandler() const override;

	private:
		static const Path WIDGET_LAYOUT_PATH;
		static const Path BUILD_DATA_PATH;
		RenderSystemPlugin mActiveRSPlugin;
		EditorSettingsPtr mEditorSettings;

		DynLib* mSBansheeEditorPlugin;

		// DEBUG ONLY

		HShader mTestShader;
		HMaterial mTestMaterial;
		HTexture mTestTexRef;
		HMesh mDbgMeshRef;
	};

	/**
	 * @brief	Returns the globally accessible instance of editor application.
	 */
	BS_ED_EXPORT EditorApplication& gEditorApplication();
}