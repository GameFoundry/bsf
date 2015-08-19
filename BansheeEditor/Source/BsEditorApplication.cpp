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

// DEBUG ONLY
#include "BsResources.h"
#include "BsSceneObject.h"
#include "BsImporter.h"
#include "BsGpuProgram.h"
#include "BsShader.h"
#include "BsTexture.h"
#include "BsMaterial.h"
#include "BsTechnique.h"
#include "BsPass.h"
#include "BsRenderable.h"
#include "BsVirtualInput.h"
#include "BsFolderMonitor.h"
#include "BsCamera.h"
#include "BsGUIWidget.h"
#include "BsGUIButton.h"
#include "BsGUILayout.h"
#include "BsEvent.h"
#include "BsCoreRenderer.h"
#include "BsEditorSettings.h"
#include "BsMesh.h"
#include "BsMath.h"
#include "BsDebug.h"

namespace BansheeEngine
{
	const Path EditorApplication::WIDGET_LAYOUT_PATH = L"Internal\\Layout.asset";
	const Path EditorApplication::BUILD_DATA_PATH = L"Internal\\BuildData.asset";

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
		mActiveRAPIPlugin(renderAPIPlugin), mSBansheeEditorPlugin(nullptr)
	{

	}

	EditorApplication::~EditorApplication()
	{
		shutdownPlugin(mSBansheeEditorPlugin);

		/************************************************************************/
		/* 								DEBUG CODE                      		*/
		/************************************************************************/

		gResources().unload(mTestTexRef);
		gResources().unload(mDbgMeshRef);
		gResources().unload(mTestShader);
		gResources().unload(mTestMaterial);

		mTestMaterial = nullptr;
		mTestTexRef = nullptr;
		mDbgMeshRef = nullptr;
		mTestShader = nullptr;

		/************************************************************************/
		/* 							END DEBUG CODE                      		*/
		/************************************************************************/

		ProjectLibrary::shutDown();
		BuiltinEditorResources::shutDown();
	}

	void EditorApplication::onStartUp()
	{
		Application::onStartUp();

		// TODO - Load project settings
		mEditorSettings = bs_shared_ptr_new<EditorSettings>();

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

		ProjectLibrary::startUp(getProjectPath());

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
		loadPlugin("SBansheeEditor", &mSBansheeEditorPlugin); // Managed part of the editor

		EditorWidgetLayoutPtr layout = loadWidgetLayout();
		if (layout != nullptr)
			EditorWidgetManager::instance().setLayout(layout);

		BuildManager::instance().load(BUILD_DATA_PATH);

		/************************************************************************/
		/* 								DEBUG CODE                      		*/
		/************************************************************************/

		HShader dummyParsedShader = Importer::instance().import<Shader>("..\\..\\..\\..\\Data\\Raw\\Engine\\Shaders\\TestFX.bsl");
		assert(dummyParsedShader != nullptr); // Ad hoc unit test

		RenderAPICore* renderAPI = RenderAPICore::instancePtr();

		HSceneObject testModelGO = SceneObject::create("TestMesh");
		HRenderable testRenderable = testModelGO->addComponent<Renderable>();

		WString testShaderLoc = L"..\\..\\..\\..\\Data\\Test.bsl";;
		
		mTestShader = Importer::instance().import<Shader>(testShaderLoc);

		gResources().save(mTestShader, L"C:\\testShader.asset", true);
		gResources().unload(mTestShader);
		mTestShader = gResources().load<Shader>(L"C:\\testShader.asset");

		mTestMaterial = Material::create(mTestShader);

		mTestTexRef = static_resource_cast<Texture>(Importer::instance().import(L"..\\..\\..\\..\\Data\\Examples\\Dragon.tga"));
		mDbgMeshRef = static_resource_cast<Mesh>(Importer::instance().import(L"..\\..\\..\\..\\Data\\Examples\\Dragon.fbx"));

		gResources().save(mTestTexRef, L"C:\\ExportTest.tex", true);
		gResources().save(mDbgMeshRef, L"C:\\ExportMesh.mesh", true);

		gResources().unload(mTestTexRef);
		gResources().unload(mDbgMeshRef);

		mTestTexRef = static_resource_cast<Texture>(gResources().loadAsync(L"C:\\ExportTest.tex"));
		mDbgMeshRef = static_resource_cast<Mesh>(gResources().loadAsync(L"C:\\ExportMesh.mesh"));

		mDbgMeshRef.blockUntilLoaded();
		mDbgMeshRef->blockUntilCoreInitialized();
		mTestTexRef.blockUntilLoaded();
		mTestTexRef->blockUntilCoreInitialized();

		mTestMaterial->setTexture("tex", mTestTexRef);
		gResources().save(mTestShader, L"C:\\ExportShader.asset", true);
		gResources().save(mTestMaterial, L"C:\\ExportMaterial.mat", true);

		gResources().unload(mTestMaterial);
		gResources().unload(mTestShader);

		mTestShader = gResources().load<Shader>(L"C:\\ExportShader.asset");
		mTestMaterial = gResources().load<Material>(L"C:\\ExportMaterial.mat");

		testRenderable->setMesh(mDbgMeshRef);
		testRenderable->setMaterial(0, mTestMaterial);

		HSceneObject clone = testModelGO->clone();
		testModelGO->destroy();

		/************************************************************************/
		/* 							END DEBUG CODE                      		*/
		/************************************************************************/
	}

	void EditorApplication::onShutDown()
	{
		BuildManager::instance().save(BUILD_DATA_PATH);

		CodeEditorManager::shutDown();
		BuildManager::shutDown();
		GizmoManager::shutDown();
		Selection::shutDown();
		ScenePicking::shutDown();

		saveWidgetLayout(EditorWidgetManager::instance().getLayout());
		// TODO - Save project settings

		DropDownWindowManager::shutDown();
		EditorWidgetManager::shutDown();
		EditorWindowManager::shutDown();
		UndoRedo::shutDown();

		Application::onShutDown();
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

		ProjectLibrary::instance().update();
		EditorWindowManager::instance().update();	
	}

	bool EditorApplication::isProjectLoaded() const
	{
		return true; // TODO - DEBUG ONLY
	}

	const Path& EditorApplication::getProjectPath() const
	{
		static Path dummyProjectPath = L"D:\\DummyBansheeProject\\";

		return dummyProjectPath;
	}

	const WString& EditorApplication::getProjectName() const
	{
		static WString dummyProjectName = L"DummyBansheeProject";

		return dummyProjectName;
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
		Path assemblyFolder = getProjectPath();
		assemblyFolder.append(INTERNAL_ASSEMBLY_PATH);

		return assemblyFolder;
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

	ShaderIncludeHandlerPtr EditorApplication::getShaderIncludeHandler() const
	{
		return bs_shared_ptr_new<EditorShaderIncludeHandler>();
	}

	EditorApplication& gEditorApplication()
	{
		return static_cast<EditorApplication&>(EditorApplication::instance());
	}
}