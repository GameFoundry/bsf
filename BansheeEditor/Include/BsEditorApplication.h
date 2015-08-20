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
		EditorApplication(RenderAPIPlugin renderAPI);
		virtual ~EditorApplication();

		/**
		 * @brief	Starts the editorn with the specified render system.
		 */
		static void startUp(RenderAPIPlugin renderAPI);

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

		/**
		 * @brief	Returns a set of serializable project settings that contain
		 *			every customizable property specific to a project.
		 */
		ProjectSettingsPtr getProjectSettings() const { return mProjectSettings; }

		/**
		 * @brief	Saves the current editor settings at the default location.
		 */
		void saveEditorSettings();

		/**
		 * @brief	Saves the current project settings at the default location.
		 *			Does nothing if no project is loaded.
		 */
		void saveProjectSettings();

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
		 * @brief	Loads the previously saved editor settings from the default location.
		 *			Overwrites any current settings.
		 */
		void loadEditorSettings();

		/**
		 * @brief	Loads the previously saved project settings from the default location
		 *			within the active project. Loads default settings if no project is active.
		 *			Overwrites any current settings.
		 */
		void loadProjectSettings();

		/**
		 * @copydoc	Application::getShaderIncludeHandler
		 */
		virtual ShaderIncludeHandlerPtr getShaderIncludeHandler() const override;

	private:
		static const Path WIDGET_LAYOUT_PATH;
		static const Path BUILD_DATA_PATH;
		static const Path EDITOR_SETTINGS_PATH;
		static const Path PROJECT_SETTINGS_PATH;

		RenderAPIPlugin mActiveRAPIPlugin;
		EditorSettingsPtr mEditorSettings;
		ProjectSettingsPtr mProjectSettings;

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