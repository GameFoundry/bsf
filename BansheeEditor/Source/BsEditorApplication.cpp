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
#include "BsFileSystem.h"

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
#include "BsCRenderable.h"
#include "BsVirtualInput.h"
#include "BsFolderMonitor.h"
#include "BsCCamera.h"
#include "BsCGUIWidget.h"
#include "BsGUIButton.h"
#include "BsGUILayout.h"
#include "BsEvent.h"
#include "BsCoreRenderer.h"
#include "BsMesh.h"
#include "BsMath.h"
#include "BsDebug.h"
#include "../../BansheeFreeImgImporter/Dependencies/Include/FreeImage.h"

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

		/************************************************************************/
		/* 								DEBUG CODE                      		*/
		/************************************************************************/

		HShader dummyParsedShader = Importer::instance().import<Shader>(RUNTIME_DATA_PATH + "Raw\\Engine\\Shaders\\TestFX.bsl");
		assert(dummyParsedShader != nullptr); // Ad hoc unit test

		RenderAPICore* renderAPI = RenderAPICore::instancePtr();

		HSceneObject testModelGO = SceneObject::create("TestMesh");
		HRenderable testRenderable = testModelGO->addComponent<CRenderable>();

		Path testShaderLoc = RUNTIME_DATA_PATH + L"Test.bsl";;

		mTestShader = Importer::instance().import<Shader>(testShaderLoc);

		gResources().save(mTestShader, L"E:\\testShader.asset", true);
		gResources().unload(mTestShader);
		mTestShader = gResources().load<Shader>(L"E:\\testShader.asset");

		mTestMaterial = Material::create(mTestShader);

		mTestTexRef = static_resource_cast<Texture>(Importer::instance().import(RUNTIME_DATA_PATH + L"Examples\\Dragon.tga"));
		mDbgMeshRef = static_resource_cast<Mesh>(Importer::instance().import(RUNTIME_DATA_PATH + L"Examples\\Dragon.fbx"));

		gResources().save(mTestTexRef, L"E:\\ExportTest.tex", true);
		gResources().save(mDbgMeshRef, L"E:\\ExportMesh.mesh", true);

		gResources().unload(mTestTexRef);
		gResources().unload(mDbgMeshRef);

		mTestTexRef = static_resource_cast<Texture>(gResources().loadAsync(L"E:\\ExportTest.tex"));
		mDbgMeshRef = static_resource_cast<Mesh>(gResources().loadAsync(L"E:\\ExportMesh.mesh"));

		mDbgMeshRef.blockUntilLoaded();
		mDbgMeshRef->blockUntilCoreInitialized();
		mTestTexRef.blockUntilLoaded();
		mTestTexRef->blockUntilCoreInitialized();

		mTestMaterial->setTexture("tex", mTestTexRef);
		gResources().save(mTestShader, L"E:\\ExportShader.asset", true);
		gResources().save(mTestMaterial, L"E:\\ExportMaterial.mat", true);

		gResources().unload(mTestMaterial);
		gResources().unload(mTestShader);

		mTestShader = gResources().load<Shader>(L"E:\\ExportShader.asset");
		mTestMaterial = gResources().load<Material>(L"E:\\ExportMaterial.mat");

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

	// DEBUG ONLY
	
	struct MSDOSHeader
	{
		UINT16 signature;
		UINT16 lastSize;
		UINT16 numBlocks;
		UINT16 numReloc;
		UINT16 hdrSize;
		UINT16 minAlloc;
		UINT16 maxAlloc;
		UINT16 ss;
		UINT16 sp;
		UINT16 checksum;
		UINT16 ip;
		UINT16 cs;
		UINT16 relocPos;
		UINT16 numOverlay;
		UINT16 reserved1[4];
		UINT16 oemId;
		UINT16 oemInfo;
		UINT16 reserved2[10];
		UINT32 lfanew;
	};

	struct COFFHeader
	{
		UINT16 machine;
		UINT16 numSections;
		UINT32 timeDateStamp;
		UINT32 ptrSymbolTable;
		UINT32 numSymbols;
		UINT16 sizeOptHeader;
		UINT16 characteristics;
	};

	struct PEDataDirectory
	{
		UINT32 virtualAddress;
		UINT32 size;
	};

	struct PEOptionalHeader32
	{
		UINT16 signature; 
		UINT8 majorLinkerVersion;
		UINT8 minorLinkerVersion;
		UINT32 sizeCode;
		UINT32 sizeInitializedData;
		UINT32 sizeUninitializedData;
		UINT32 addressEntryPoint;
		UINT32 baseCode;
		UINT32 baseData;
		UINT32 baseImage;
		UINT32 alignmentSection;
		UINT32 alignmentFile;
		UINT16 majorOSVersion;
		UINT16 minorOSVersion;
		UINT16 majorImageVersion;
		UINT16 minorImageVersion;
		UINT16 majorSubsystemVersion;
		UINT16 minorSubsystemVersion;
		UINT32 reserved;
		UINT32 sizeImage;
		UINT32 sizeHeaders;
		UINT32 checksum;
		UINT16 subsystem;
		UINT16 characteristics;
		UINT32 sizeStackReserve;
		UINT32 sizeStackCommit;
		UINT32 sizeHeapReserve;
		UINT32 sizeHeapCommit;
		UINT32 loaderFlags;
		UINT32 NumRvaAndSizes;
		PEDataDirectory dataDirectory[16];
	};

	struct PEOptionalHeader64
	{
		UINT16 signature;
		UINT8 majorLinkerVersion;
		UINT8 minorLinkerVersion;
		UINT32 sizeCode;
		UINT32 sizeInitializedData;
		UINT32 sizeUninitializedData;
		UINT32 addressEntryPoint;
		UINT32 baseCode;
		UINT64 baseImage;
		UINT32 alignmentSection;
		UINT32 alignmentFile;
		UINT16 majorOSVersion;
		UINT16 minorOSVersion;
		UINT16 majorImageVersion;
		UINT16 minorImageVersion;
		UINT16 majorSubsystemVersion;
		UINT16 minorSubsystemVersion;
		UINT32 reserved;
		UINT32 sizeImage;
		UINT32 sizeHeaders;
		UINT32 checksum;
		UINT16 subsystem;
		UINT16 characteristics;
		UINT64 sizeStackReserve;
		UINT64 sizeStackCommit;
		UINT64 sizeHeapReserve;
		UINT64 sizeHeapCommit;
		UINT32 loaderFlags;
		UINT32 NumRvaAndSizes;
		PEDataDirectory dataDirectory[16];
	};

	struct PESectionHeader
	{
		char name[8];
		UINT32 virtualSize;
		UINT32 relativeVirtualAddress;
		UINT32 physicalSize;
		UINT32 physicalAddress;
		UINT8 deprecated[12];
		UINT32 flags;
	};

	struct PEImageResourceDirectory 
	{
		UINT32 flags;
		UINT32 timeDateStamp;
		UINT16 majorVersion;
		UINT16 minorVersion;
		UINT16 numNamedEntries;
		UINT16 numIdEntries;
	};

	struct PEImageResourceEntryData
	{
		UINT32 offsetData;
		UINT32 size;
		UINT32 codePage;
		UINT32 resourceHandle;
	};

	struct PEImageResourceEntry
	{
		UINT32 offsetName;
		UINT32 type;
		UINT32 offsetDirectory : 31;
		UINT32 isDirectory : 1;
	};

#define MSDOS_SIGNATURE 0x5A4D
#define PE_SIGNATURE 0x0000455
#define PE_32BIT_SIGNATURE 0x10B
#define PE_64BIT_SIGNATURE 0x20B
#define PE_NUM_DIRECTORY_ENTRIES 16
#define PE_SECTION_UNINITIALIZED_DATA 0x00000080
#define	PE_IMAGE_DIRECTORY_ENTRY_RESOURCE 2
#define PE_IMAGE_RT_ICON 3

	struct IconHeader
	{
		UINT32 size;
		INT32 width;
		INT32 height;
		UINT16 planes;
		UINT16 bitCount;
		UINT32 compression;
		UINT32 sizeImage;
		INT32 xPelsPerMeter;
		INT32 yPelsPerMeter;
		UINT32 clrUsed;
		UINT32 clrImportant;
	};

	void copyPixelsToIcon(const Map<UINT32, PixelDataPtr>& pixelsPerSize, UINT8* iconData)
	{
		IconHeader* iconHeader = (IconHeader*)iconData;

		if (iconHeader->size != sizeof(IconHeader) || iconHeader->compression != 0 
			|| iconHeader->planes != 1 || iconHeader->bitCount != 32)
		{
			// Unsupported format
			return;
		}

		UINT8* iconPixels = iconData + sizeof(IconHeader);
		UINT32 width = iconHeader->width;
		UINT32 height = iconHeader->height / 2;

		auto iterFind = pixelsPerSize.find(width);
		if (iterFind == pixelsPerSize.end() || iterFind->second->getWidth() != width 
			|| iterFind->second->getHeight() != height)
		{
			// No icon of this size provided
			return;
		}

		// Write colors
		PixelDataPtr srcPixels = iterFind->second;
		UINT32* colorData = (UINT32*)iconPixels;

		UINT32 idx = 0;
		for (UINT32 y = 0; y < height; y++)
		{
			for (UINT32 x = 0; x < width; x++)
				colorData[idx] = srcPixels->getColorAt(x, y).getAsBGRA();
		}

		// Write AND mask
		UINT32 colorDataSize = width * height * sizeof(UINT32);
		UINT8* maskData = iconPixels + colorDataSize;

		UINT32 numPackedPixels = width / 8; // One per bit in byte

		for (UINT32 y = 0; y < height; y++)
		{
			UINT8 mask = 0;
			for (UINT32 packedX = 0; packedX < numPackedPixels; packedX++)
			{
				for (UINT32 pixelIdx = 0; pixelIdx < 8; pixelIdx++)
				{
					UINT32 x = packedX * 8 + pixelIdx;
					Color color = srcPixels->getColorAt(x, y);
					if (color.a < 0.25f)
						mask |= 1 << 7 - pixelIdx;
				}

				*maskData = mask;
				maskData++;
			}
		}
	}

	void setIconInEXE(const Path& path, const Map<UINT32, PixelDataPtr>& pixelsPerSize)
	{
		// A PE file is structured as such:
		//  - MSDOS Header
		//  - PE Signature
		//  - COFF Header
		//  - PE Optional Header
		//  - One or multiple sections
		//   - .code
		//   - .data
		//   - ...
		//   - .rsrc
		//    - icon/cursor/etc data

		std::fstream stream;
		stream.open(path.toString().c_str(), std::ios::in | std::ios::out | std::ios::binary);
		
		// First check magic number to ensure file is even an executable
		UINT16 magicNum;
		stream.read((char*)&magicNum, sizeof(magicNum));
		if (magicNum != MSDOS_SIGNATURE)
			BS_EXCEPT(InvalidStateException, "Provided file is not a valid executable.");

		// Read the MSDOS header and skip over it
		stream.seekg(0);

		MSDOSHeader msdosHeader;
		stream.read((char*)&msdosHeader, sizeof(MSDOSHeader));

		// Read PE signature
		stream.seekg(msdosHeader.lfanew);

		UINT32 peSignature;
		stream.read((char*)&peSignature, sizeof(peSignature));

		if (peSignature != PE_SIGNATURE)
			BS_EXCEPT(InvalidStateException, "Provided file is not in PE format.");

		// Read COFF header
		COFFHeader coffHeader;
		stream.read((char*)&coffHeader, sizeof(COFFHeader));

		if (coffHeader.sizeOptHeader == 0) // .exe files always have an optional header
			BS_EXCEPT(InvalidStateException, "Provided file is not a valid executable.");

		UINT32 numSectionHeaders = coffHeader.numSections;

		// Read optional header
		auto optionalHeaderPos = stream.tellg();

		UINT16 optionalHeaderSignature;
		stream.read((char*)&optionalHeaderSignature, sizeof(optionalHeaderSignature));

		PEDataDirectory* dataDirectory = nullptr;
		stream.seekg(optionalHeaderPos);
		if (optionalHeaderSignature == PE_32BIT_SIGNATURE)
		{
			PEOptionalHeader32 optionalHeader;
			stream.read((char*)&optionalHeader, sizeof(optionalHeader));

			dataDirectory = optionalHeader.dataDirectory;
		}
		else if (optionalHeaderSignature == PE_64BIT_SIGNATURE)
		{
			PEOptionalHeader64 optionalHeader;
			stream.read((char*)&optionalHeader, sizeof(optionalHeader));

			dataDirectory = optionalHeader.dataDirectory;
		}
		else
			BS_EXCEPT(InvalidStateException, "Unrecognized PE format.");

		// Read section headers
		auto sectionHeaderPos = optionalHeaderPos + (std::ifstream::pos_type)coffHeader.sizeOptHeader;
		stream.seekg(sectionHeaderPos);

		PESectionHeader* sectionHeaders = bs_stack_alloc<PESectionHeader>(numSectionHeaders);
		stream.read((char*)sectionHeaders, sizeof(PESectionHeader) * numSectionHeaders);

		// Look for .rsrc section header
		std::function<void(PEImageResourceDirectory*, PEImageResourceDirectory*, UINT8*, UINT32)> setIconData =
			[&](PEImageResourceDirectory* base, PEImageResourceDirectory* current, UINT8* imageData, UINT32 sectionAddress)
		{
			UINT32 numEntries = current->numIdEntries + current->numNamedEntries;
			PEImageResourceEntry* entries = (PEImageResourceEntry*)(current + 1);

			for (UINT32 i = 0; i < numEntries; i++)
			{
				if (entries[i].type != PE_IMAGE_RT_ICON)
					continue;

				if(entries[i].isDirectory)
				{
					PEImageResourceDirectory* child = (PEImageResourceDirectory*)(((UINT8*)base) + entries[i].offsetDirectory);
					setIconData(base, child, imageData, sectionAddress);
				}
				else
				{
					PEImageResourceEntryData* data = (PEImageResourceEntryData*)(((UINT8*)base) + entries[i].offsetDirectory);

					UINT8* iconData = imageData + (data->offsetData - sectionAddress);
					copyPixelsToIcon(pixelsPerSize, iconData);
				}
			}
		};

		for (UINT32 i = 0; i < numSectionHeaders; i++)
		{
			if (sectionHeaders[i].flags & PE_SECTION_UNINITIALIZED_DATA)
				continue;

			if (strcmp(sectionHeaders[i].name, ".rsrc") == 0)
			{
				UINT32 imageSize = sectionHeaders[i].physicalSize;
				UINT8* imageData = (UINT8*)bs_stack_alloc(imageSize);

				stream.seekg(sectionHeaders[i].physicalAddress);
				stream.read((char*)imageData, imageSize);

				UINT32 resourceDirOffset = dataDirectory->virtualAddress - sectionHeaders[i].relativeVirtualAddress;
				PEImageResourceDirectory* resourceDirectory = (PEImageResourceDirectory*)&imageData[resourceDirOffset];

				setIconData(resourceDirectory, resourceDirectory, imageData, sectionHeaders[i].relativeVirtualAddress);
				stream.seekp(sectionHeaders[i].physicalAddress);
				stream.write((char*)imageData, imageSize);

				bs_stack_free(imageData);
			}
		}

		bs_stack_free(sectionHeaders);
		stream.close();
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

		ProjectLibrary::instance().saveLibrary();
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
		ProjectLibrary::instance().unloadLibrary();
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
		ProjectLibrary::instance().loadLibrary();

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