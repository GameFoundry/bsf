#include "BsEditorApplication.h"
#include "BsEditorWindowManager.h"
#include "BsEditorWidgetManager.h"
#include "BsMainEditorWindow.h"
#include "BsRenderWindow.h"
#include "BsBuiltinEditorResources.h"
#include "BsUndoRedo.h"
#include "BsFileSerializer.h"
#include "BsFileSystem.h"
#include "BsResourceImporter.h"
#include "BsEditorWidgetLayout.h"
#include "BsScenePicking.h"
#include "BsSelection.h"
#include "BsGizmoManager.h"
#include "BsCodeEditor.h"
#include "BsBuildManager.h"
#include "BsScriptCodeImporter.h"
#include "BsShaderIncludeHandler.h"
#include "BsDropDownWindowManager.h"
#include "BsPrefabImporter.h"
#include "BsProjectLibrary.h"
#include "BsProjectSettings.h"
#include "BsEditorSettings.h"
#include "BsScriptManager.h"
#include "BsImporter.h"
#include "BsVirtualInput.h"
#include "BsResources.h"
#include "BsCoreSceneManager.h"

// DEBUG ONLY
#include "BsShader.h"

namespace BansheeEngine
{
	const Path EditorApplication::WIDGET_LAYOUT_PATH = PROJECT_INTERNAL_DIR + L"Layout.asset";
	const Path EditorApplication::BUILD_DATA_PATH = PROJECT_INTERNAL_DIR + L"BuildData.asset";
	const Path EditorApplication::EDITOR_SETTINGS_PATH = RUNTIME_DATA_PATH + L"Settings.asset";
	const Path EditorApplication::PROJECT_SETTINGS_PATH = PROJECT_INTERNAL_DIR + L"Settings.asset";

	RENDER_WINDOW_DESC createRenderWindowDesc()
	{
		RENDER_WINDOW_DESC renderWindowDesc;
		renderWindowDesc.videoMode = VideoMode(1280, 720);
		renderWindowDesc.title = "BansheeEditor";
		renderWindowDesc.fullscreen = false;
		renderWindowDesc.border = WindowBorder::None;
		renderWindowDesc.hideUntilSwap = true;

		return renderWindowDesc;
	}

	EditorApplication::EditorApplication(RenderAPIPlugin renderAPIPlugin)
		:Application(createRenderWindowDesc(), renderAPIPlugin, RendererPlugin::Default),
		mActiveRAPIPlugin(renderAPIPlugin), mSBansheeEditorPlugin(nullptr), mIsProjectLoaded(false)
	{

	}

	EditorApplication::~EditorApplication()
	{
		ProjectLibrary::shutDown();
		BuiltinEditorResources::shutDown();
	}

	void EditorApplication::onStartUp()
	{
		Application::onStartUp();

		loadEditorSettings();
		mProjectSettings = bs_shared_ptr_new<ProjectSettings>();

		BuiltinEditorResources::startUp();

		{
			auto inputConfig = VirtualInput::instance().getConfiguration();

			inputConfig->registerButton("Rename", BC_F2);
			inputConfig->registerButton("Undo", BC_Z, ButtonModifier::Ctrl);
			inputConfig->registerButton("Redo", BC_Y, ButtonModifier::Ctrl);
			inputConfig->registerButton("Copy", BC_C, ButtonModifier::Ctrl);
			inputConfig->registerButton("Cut", BC_X, ButtonModifier::Ctrl);
			inputConfig->registerButton("Paste", BC_V, ButtonModifier::Ctrl);
			inputConfig->registerButton("Duplicate", BC_D, ButtonModifier::Ctrl);
			inputConfig->registerButton("Delete", BC_DELETE);
		}

		ScriptCodeImporter* scriptCodeImporter = bs_new<ScriptCodeImporter>();
		Importer::instance()._registerAssetImporter(scriptCodeImporter);

		ResourceImporter* resourceImporter = bs_new<ResourceImporter>();
		Importer::instance()._registerAssetImporter(resourceImporter);

		PrefabImporter* prefabImporter = bs_new<PrefabImporter>();
		Importer::instance()._registerAssetImporter(prefabImporter);

		ProjectLibrary::startUp();

		UndoRedo::startUp();
		EditorWindowManager::startUp();
		EditorWidgetManager::startUp();
		DropDownWindowManager::startUp();

		ScenePicking::startUp();
		Selection::startUp();
		GizmoManager::startUp();
		BuildManager::startUp();
		CodeEditorManager::startUp();

		MainEditorWindow* mainWindow = MainEditorWindow::create(getPrimaryWindow());
		ScriptManager::instance().initialize();

#if BS_DEBUG_MODE
		HShader dummyParsedShader = Importer::instance().import<Shader>(RUNTIME_DATA_PATH + "Raw\\Engine\\Shaders\\TestFX.bsl");
		assert(dummyParsedShader != nullptr); // Ad hoc unit test
#endif
	}

	void EditorApplication::onShutDown()
	{
		unloadProject();

		CodeEditorManager::shutDown();
		BuildManager::shutDown();
		GizmoManager::shutDown();
		Selection::shutDown();
		ScenePicking::shutDown();

		saveEditorSettings();

		DropDownWindowManager::shutDown();
		EditorWidgetManager::shutDown();
		EditorWindowManager::shutDown();
		UndoRedo::shutDown();

		Application::onShutDown();
	}

	void EditorApplication::loadScriptSystem()
	{
		loadPlugin("BansheeMono", &mMonoPlugin);
		loadPlugin("SBansheeEngine", &mSBansheeEnginePlugin);
		loadPlugin("SBansheeEditor", &mSBansheeEditorPlugin);
	}

	void EditorApplication::startUp(RenderAPIPlugin renderAPI)
	{
		CoreApplication::startUp<EditorApplication>(renderAPI);
	}

	void EditorApplication::preUpdate()
	{
		Application::preUpdate();

		EditorWidgetManager::instance().update();
		DropDownWindowManager::instance().update();
	}

	void EditorApplication::postUpdate()
	{
		Application::postUpdate();

		gProjectLibrary().update();
		EditorWindowManager::instance().update();	
	}

	Path EditorApplication::getEditorAssemblyPath() const
	{
		Path assemblyPath = getBuiltinAssemblyFolder();
		assemblyPath.append(toWString(EDITOR_ASSEMBLY) + L".dll");

		return assemblyPath;
	}

	Path EditorApplication::getEditorScriptAssemblyPath() const
	{
		Path assemblyPath = getScriptAssemblyFolder();
		assemblyPath.append(toWString(SCRIPT_EDITOR_ASSEMBLY) + L".dll");

		return assemblyPath;
	}

	Path EditorApplication::getScriptAssemblyFolder() const
	{
		if (!isProjectLoaded())
			return Path::BLANK;

		Path assemblyFolder = getProjectPath();
		assemblyFolder.append(INTERNAL_ASSEMBLY_PATH);

		return assemblyFolder;
	}

	void EditorApplication::saveProject()
	{
		if (!isProjectLoaded())
			return;

		Path buildDataPath = getProjectPath();
		buildDataPath.append(BUILD_DATA_PATH);

		BuildManager::instance().save(buildDataPath);
		saveWidgetLayout(EditorWidgetManager::instance().getLayout());
		saveEditorSettings();
		saveProjectSettings();

		gProjectLibrary().saveLibrary();
	}

	void EditorApplication::unloadProject()
	{
		if (!isProjectLoaded())
			return;

		saveProject();

		mProjectSettings = bs_shared_ptr_new<ProjectSettings>();
		BuildManager::instance().clear();
		UndoRedo::instance().clear();

		EditorWidgetManager::instance().closeAll();
		gProjectLibrary().unloadLibrary();
		Resources::instance().unloadAllUnused();
		gCoreSceneManager().clearScene();

		mProjectPath = Path::BLANK;
		mProjectName = StringUtil::WBLANK;
		mIsProjectLoaded = false;
	}

	void EditorApplication::loadProject(const Path& projectPath)
	{
		unloadProject();

		mProjectPath = projectPath;
		mProjectName = projectPath.getWTail();
		mIsProjectLoaded = true;

		loadProjectSettings();

		Path buildDataPath = getProjectPath();
		buildDataPath.append(BUILD_DATA_PATH);

		BuildManager::instance().load(buildDataPath);

		// Do this before restoring windows and loading library to ensure types are loaded
		ScriptManager::instance().reload();
		gProjectLibrary().loadLibrary();

		EditorWidgetLayoutPtr layout = loadWidgetLayout();
		if (layout != nullptr)
			EditorWidgetManager::instance().setLayout(layout);
	}

	void EditorApplication::createProject(const Path& path)
	{
		Path resourceDir = Path::combine(path, ProjectLibrary::RESOURCES_DIR);
		Path internalResourcesDir = Path::combine(path, ProjectLibrary::INTERNAL_RESOURCES_DIR);

		if (!FileSystem::exists(resourceDir))
			FileSystem::createDir(resourceDir);

		if (!FileSystem::exists(internalResourcesDir))
			FileSystem::createDir(internalResourcesDir);

		Path defaultLayoutPath = FileSystem::getWorkingDirectoryPath();
		defaultLayoutPath.append(BuiltinEditorResources::DefaultWidgetLayoutPath);

		if (FileSystem::exists(defaultLayoutPath))
		{
			Path projectLayoutPath = Path::combine(path, WIDGET_LAYOUT_PATH);
			FileSystem::copy(defaultLayoutPath, projectLayoutPath, false);
		}
	}

	bool EditorApplication::isValidProjectPath(const Path& path)
	{
		if (!path.isAbsolute())
			return false;

		if (!FileSystem::isDirectory(path))
			return false;

		return true;
	}

	EditorWidgetLayoutPtr EditorApplication::loadWidgetLayout()
	{
		Path layoutPath = getProjectPath();
		layoutPath.append(WIDGET_LAYOUT_PATH);

		if(FileSystem::exists(layoutPath))
		{
			FileDecoder fs(layoutPath);
			return std::static_pointer_cast<EditorWidgetLayout>(fs.decode());
		}

		return nullptr;
	}

	void EditorApplication::saveWidgetLayout(const EditorWidgetLayoutPtr& layout)
	{
		Path layoutPath = getProjectPath();
		layoutPath.append(WIDGET_LAYOUT_PATH);

		FileEncoder fs(layoutPath);
		fs.encode(layout.get());
	}

	void EditorApplication::loadEditorSettings()
	{
		Path absoluteDataPath = FileSystem::getWorkingDirectoryPath();
		absoluteDataPath.append(EDITOR_SETTINGS_PATH);

		if (FileSystem::exists(absoluteDataPath))
		{
			FileDecoder fs(absoluteDataPath);
			mEditorSettings = std::static_pointer_cast<EditorSettings>(fs.decode());
		}
		
		if (mEditorSettings == nullptr)
			mEditorSettings = bs_shared_ptr_new<EditorSettings>();
	}

	void EditorApplication::saveEditorSettings()
	{
		if (mEditorSettings == nullptr)
			return;

		Path absoluteDataPath = FileSystem::getWorkingDirectoryPath();
		absoluteDataPath.append(EDITOR_SETTINGS_PATH);

		FileEncoder fs(absoluteDataPath);
		fs.encode(mEditorSettings.get());
	}

	void EditorApplication::loadProjectSettings()
	{
		if (isProjectLoaded())
		{
			Path absoluteDataPath = getProjectPath();
			absoluteDataPath.append(PROJECT_SETTINGS_PATH);

			if (FileSystem::exists(absoluteDataPath))
			{
				FileDecoder fs(absoluteDataPath);
				mProjectSettings = std::static_pointer_cast<ProjectSettings>(fs.decode());
			}
		}

		if (mProjectSettings == nullptr)
			mProjectSettings = bs_shared_ptr_new<ProjectSettings>();
	}

	void EditorApplication::saveProjectSettings()
	{
		if (mProjectSettings == nullptr || !isProjectLoaded())
			return;

		Path absoluteDataPath = getProjectPath();
		absoluteDataPath.append(PROJECT_SETTINGS_PATH);

		FileEncoder fs(absoluteDataPath);
		fs.encode(mProjectSettings.get());
	}

	ShaderIncludeHandlerPtr EditorApplication::getShaderIncludeHandler() const
	{
		return bs_shared_ptr_new<EditorShaderIncludeHandler>();
	}

	EditorApplication& gEditorApplication()
	{
		return static_cast<EditorApplication&>(EditorApplication::instance());
	}
}