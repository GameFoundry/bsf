#pragma once

#include "BsEditorPrerequisites.h"
#include "BsApplication.h"

namespace BansheeEngine
{
	/**
	 * @brief	Types of render APIs supported by the editor.
	 */
	enum class EditorRenderAPI
	{
		DX11,
		OpenGL
	};

	/**
	 * @brief	Primary editor class containing the editor entry point.
	 */
	class BS_ED_EXPORT EditorApplication : public Application
	{
	public:
		EditorApplication(EditorRenderAPI renderAPI);
		virtual ~EditorApplication();

		/**
		 * @brief	Starts the editor with the specified render system.
		 */
		static void startUp(EditorRenderAPI renderAPI);

		/**
		 * @brief	Checks whether the editor currently has a project loaded.
		 */
		bool isProjectLoaded() const { return mIsProjectLoaded; }

		/**
		 * @brief	Returns the path to the currently loaded project.
		 */
		const Path& getProjectPath() const { return mProjectPath; }

		/**
		 * @brief	Returns the name of the currently loaded project.
		 */
		const WString& getProjectName() const { return mProjectName; }

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

		/**
		 * @brief	Saves any project specific data, if a project is currently loaded.
		 */
		void saveProject();

		/**
		 * @brief	Unloads the currently loaded project, if any.
		 */
		void unloadProject();

		/**
		 * @brief	Loads a new project, unloading the current one. 
		 *
		 * @param	path	Absolute path to the root project folder. Must be pointing
		 *					to a valid project.
		 */
		void loadProject(const Path& path);

		/**
		 * @brief	Creates a new project at the specified path.
		 *
		 * @param	path	Path to the folder where to create the project in. Name of this
		 *					folder will be the name of the project.
		 */
		void createProject(const Path& path);

		/**
		 * @brief	Checks is the provided folder a valid project.
		 *
		 * @param	path	Absolute path to the root project folder.
		 */
		bool isValidProjectPath(const Path& path);
	private:
		/**
		 * @copydoc	Module::onStartUp
		 */
		virtual void onStartUp() override;

		/**
		 * @copydoc	Module::onShutDown
		 */
		virtual void onShutDown() override;

		/**
		 * @copydoc	Module::preUpdate
		 */
		virtual void preUpdate() override;

		/**
		 * @copydoc	Module::postUpdate
		 */
		virtual void postUpdate() override;

		/**
		 * @copydoc	Application::loadScriptSystem
		 */
		void loadScriptSystem() override;

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

		/**
		 * @brief	Converts a render API type supported by the editor into a type recognized by the lower
		 * 			layers of the engine.
		 */
		static RenderAPIPlugin toEngineRenderAPI(EditorRenderAPI renderAPI);

	private:
		static const Path WIDGET_LAYOUT_PATH;
		static const Path BUILD_DATA_PATH;
		static const Path EDITOR_SETTINGS_PATH;
		static const Path PROJECT_SETTINGS_PATH;

		RenderAPIPlugin mActiveRAPIPlugin;
		EditorSettingsPtr mEditorSettings;
		ProjectSettingsPtr mProjectSettings;

		bool mIsProjectLoaded;
		Path mProjectPath;
		WString mProjectName;

		DynLib* mSBansheeEditorPlugin;
	};

	/**
	 * @brief	Returns the globally accessible instance of editor application.
	 */
	BS_ED_EXPORT EditorApplication& gEditorApplication();
}