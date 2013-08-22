#include "stdafx.h"
#include <windows.h>

#include "BsApplication.h"
#include "CmApplication.h"
#include "CmDynLibManager.h"

#include "CmSceneObject.h"
#include "BsCamera.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmRenderSystem.h"
#include "CmRenderWindow.h"
#include "CmResources.h"
#include "BsRenderable.h"
#include "CmMaterial.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmImporter.h"
#include "CmMesh.h"
#include "CmGpuProgInclude.h" // DEBUG ONLY
#include "CmGpuProgramImportOptions.h"
#include "CmFontImportOptions.h"
#include "CmCommandQueue.h"
#include "CmBlendState.h"
#include "BsDebugDraw.h"

// Editor includes
#include "BsEditorWindowManager.h"
#include "BsMainEditorWindow.h"
// End editor includes


#include "CmDebugCamera.h"
#include "DbgEditorWidget1.h"
#include "DbgEditorWidget2.h"
#include "CmRTTIType.h"
#include "CmCursor.h"

//#define DX11
//#define DX9
#define GL

using namespace CamelotFramework;
using namespace BansheeEditor;
using namespace BansheeEngine;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	RENDER_WINDOW_DESC renderWindowDesc;
	renderWindowDesc.width = 1280;
	renderWindowDesc.height = 720;
	renderWindowDesc.title = "Banshee";
	renderWindowDesc.fullscreen = false;
	renderWindowDesc.border = WindowBorder::None;

#ifdef DX11
	gBansheeApp().startUp(renderWindowDesc, "CamelotD3D11RenderSystem", "BansheeForwardRenderer", "D:\\CamelotResourceMetas");
#elif defined DX9
	gBansheeApp().startUp(renderWindowDesc, "CamelotD3D9RenderSystem", "BansheeForwardRenderer", "D:\\CamelotResourceMetas");
#else
	gBansheeApp().startUp(renderWindowDesc, "CamelotGLRenderSystem", "BansheeForwardRenderer", "D:\\CamelotResourceMetas");
#endif

	RenderSystem* renderSystem = RenderSystem::instancePtr();
	RenderWindowPtr renderWindow = gApplication().getPrimaryWindow();

	HSceneObject testModelGO = SceneObject::create("TestMesh");
	HRenderable testRenderable = testModelGO->addComponent<Renderable>();

#if defined DX9
	///////////////// HLSL 9 SHADERS //////////////////////////
	String dx9psLoc = "C:\\Projects\\BansheeEngine\\Data\\hlsl9_ps.gpuprog";
	String dx9vsLoc = "C:\\Projects\\BansheeEngine\\Data\\hlsl9_vs.gpuprog";

	ImportOptionsPtr gpuProgImportOptions = Importer::instance().createImportOptions(dx9psLoc);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("ps_main");
		importOptions->setLanguage("hlsl");
		importOptions->setProfile(GPP_PS_2_0);
		importOptions->setType(GPT_FRAGMENT_PROGRAM);
	}

	HHighLevelGpuProgram fragProgRef = Importer::instance().import(dx9psLoc, gpuProgImportOptions);

	gpuProgImportOptions = Importer::instance().createImportOptions(dx9vsLoc);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("vs_main");
		importOptions->setLanguage("hlsl");
		importOptions->setProfile(GPP_VS_2_0);
		importOptions->setType(GPT_VERTEX_PROGRAM);
	}

	HHighLevelGpuProgram vertProgRef = Importer::instance().import(dx9vsLoc, gpuProgImportOptions);

#elif defined DX11

	HGpuProgInclude gpuProgInclude = Importer::instance().import("C:\\testInclude.gpuproginc");
	const String& debugString = gpuProgInclude->getString();
	
	/////////////////// HLSL 11 SHADERS //////////////////////////
	String dx11psLoc = "C:\\Projects\\BansheeEngine\\Data\\hlsl11_ps.gpuprog";
	String dx11vsLoc = "C:\\Projects\\BansheeEngine\\Data\\hlsl11_vs.gpuprog";

	ImportOptionsPtr gpuProgImportOptions = Importer::instance().createImportOptions(dx11psLoc);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("ps_main");
		importOptions->setLanguage("hlsl");
		importOptions->setProfile(GPP_PS_4_0);
		importOptions->setType(GPT_FRAGMENT_PROGRAM);
	}

	HHighLevelGpuProgram fragProgRef = Importer::instance().import(dx11psLoc, gpuProgImportOptions);

	gpuProgImportOptions = Importer::instance().createImportOptions(dx11vsLoc);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("vs_main");
		importOptions->setLanguage("hlsl");
		importOptions->setProfile(GPP_VS_4_0);
		importOptions->setType(GPT_VERTEX_PROGRAM);
	}

	HHighLevelGpuProgram vertProgRef = Importer::instance().import(dx11vsLoc, gpuProgImportOptions);
	
#else
	///////////////// GLSL SHADERS ////////////////////////////
	String glslpsLoc = "C:\\Projects\\BansheeEngine\\Data\\glsl_ps.gpuprog";
	String glslvsLoc = "C:\\Projects\\BansheeEngine\\Data\\glsl_vs.gpuprog";

	ImportOptionsPtr gpuProgImportOptions = Importer::instance().createImportOptions(glslpsLoc);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("main");
		importOptions->setLanguage("glsl");
		importOptions->setProfile(GPP_PS_2_0);
		importOptions->setType(GPT_FRAGMENT_PROGRAM);
	}

	HHighLevelGpuProgram fragProgRef = Importer::instance().import(glslpsLoc, gpuProgImportOptions);

	gpuProgImportOptions = Importer::instance().createImportOptions(glslvsLoc);
	if(rtti_is_of_type<GpuProgramImportOptions>(gpuProgImportOptions))
	{
		GpuProgramImportOptions* importOptions = static_cast<GpuProgramImportOptions*>(gpuProgImportOptions.get());

		importOptions->setEntryPoint("main");
		importOptions->setLanguage("glsl");
		importOptions->setProfile(GPP_VS_2_0);
		importOptions->setType(GPT_VERTEX_PROGRAM);
	}

	HHighLevelGpuProgram vertProgRef = Importer::instance().import(glslvsLoc, gpuProgImportOptions);
#endif

	gResources().create(vertProgRef, "C:\\vertProgCg.vprog", true);
	gResources().unload(vertProgRef);
	vertProgRef = gResources().load("C:\\vertProgCg.vprog");

	gResources().create(fragProgRef, "C:\\fragProgCg.vprog", true);
	gResources().unload(fragProgRef);
	fragProgRef = gResources().load("C:\\fragProgCg.vprog");

	ShaderPtr testShader = Shader::create("TestShader");

	testShader->addParameter("matViewProjection", "matViewProjection", GPDT_MATRIX_4X4);

#if defined DX11
	testShader->addParameter("input", "input", GPDT_STRUCT, 2, 8);
#endif

	testShader->addParameter("samp", "samp", GPOT_SAMPLER2D);
	testShader->addParameter("tex", "tex", GPOT_TEXTURE2D);
	TechniquePtr newTechniqueGL = testShader->addTechnique("GLRenderSystem", "ForwardRenderer");
	PassPtr newPassGL = newTechniqueGL->addPass();
	newPassGL->setVertexProgram(vertProgRef);
	newPassGL->setFragmentProgram(fragProgRef);

	// TODO - I need to create different techniques for different render systems (and renderers, if there were any),
	// which is redundant as some techniques can be reused. I should add a functionality that supports multiple
	// render systems/renderers per technique
	TechniquePtr newTechniqueDX = testShader->addTechnique("D3D9RenderSystem", "ForwardRenderer");
	PassPtr newPassDX = newTechniqueDX->addPass();
	newPassDX->setVertexProgram(vertProgRef);
	newPassDX->setFragmentProgram(fragProgRef);

	TechniquePtr newTechniqueDX11 = testShader->addTechnique("D3D11RenderSystem", "ForwardRenderer");
	PassPtr newPassDX11 = newTechniqueDX11->addPass();
	newPassDX11->setVertexProgram(vertProgRef);
	newPassDX11->setFragmentProgram(fragProgRef);

	HMaterial testMaterial = Material::create();
	testMaterial->setShader(testShader);

	testMaterial->setMat4("matViewProjection", Matrix4::IDENTITY);

#if defined DX11
	float dbgMultipliers1[2];
	dbgMultipliers1[0] = 0.0f;
	dbgMultipliers1[1] = 0.0f;

	float dbgMultipliers2[2];
	dbgMultipliers2[0] = 1.0f;
	dbgMultipliers2[1] = 1.0f;

	testMaterial->setStructData("input", dbgMultipliers1, sizeof(dbgMultipliers1), 0);
	testMaterial->setStructData("input", dbgMultipliers2, sizeof(dbgMultipliers2), 1);
#endif

	//testMaterialRef = gResources().load("C:\\testMaterial.mat");
	//testMaterialRef.waitUntilLoaded();

	/*TextureRef testTex = static_resource_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));*/
	HTexture testTexRef = static_resource_cast<Texture>(Importer::instance().import("C:\\ArenaTowerDFS.psd"));
	HMesh dbgMeshRef = static_resource_cast<Mesh>(Importer::instance().import("C:\\X_Arena_Tower.FBX"));

	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_DELAY_FREE_MEM_DF);

	gResources().create(testTexRef, "C:\\ExportTest.tex", true);
	gResources().create(dbgMeshRef, "C:\\ExportMesh.mesh", true);

	gResources().unload(testTexRef);
	gResources().unload(dbgMeshRef);

	testTexRef = static_resource_cast<Texture>(gResources().loadAsync("C:\\ExportTest.tex"));
	dbgMeshRef = static_resource_cast<Mesh>(gResources().loadAsync("C:\\ExportMesh.mesh"));
	
	dbgMeshRef.synchronize();
	testTexRef.synchronize();

	testMaterial->setTexture("tex", testTexRef);
	gResources().create(testMaterial, "C:\\ExportMaterial.mat", true);

	gResources().unload(testMaterial);

	testMaterial = gResources().load("C:\\ExportMaterial.mat");

	//_ASSERT(_CrtCheckMemory());

	testRenderable->setMesh(dbgMeshRef);
	testRenderable->setMaterial(0, testMaterial);

	//// Set the new state for the flag
	//_CrtSetDbgFlag( tmpFlag );

	HTexture dbgCursor = static_resource_cast<Texture>(Importer::instance().import("C:\\CursorDbg.psd"));
	PixelDataPtr cursorPixelData = dbgCursor->allocateSubresourceBuffer(0);

	gMainSyncedCA().readSubresource(dbgCursor.getInternalPtr(), 0, *cursorPixelData);
	gMainSyncedCA().submitToCoreThread(true);

	//Cursor::setCustomCursor(*cursorPixelData, Int2(0, 0));

	dbgCursor.reset();

	DebugDraw::instance().drawQuad2D(Vector2(-1.0f, 0.0f), Vector2(100, 50), Color::White);

	/************************************************************************/
	/* 								EDITOR INIT                      		*/
	/************************************************************************/

	MainEditorWindow* mainWindow = cm_new<MainEditorWindow>(gApplication().getPrimaryWindow());
	EditorWindowManager::startUp(cm_new<EditorWindowManager>());

	/************************************************************************/
	/* 							  EDITOR INIT END                    		*/
	/************************************************************************/

	DbgEditorWidget1::open();
	DbgEditorWidget2::open();

	gBansheeApp().runMainLoop();

	/************************************************************************/
	/* 							EDITOR SHUTDOWN                      		*/
	/************************************************************************/

	EditorWindowManager::shutDown();
	cm_delete(mainWindow);

	/************************************************************************/
	/* 							EDITOR SHUTDOWN END                    		*/
	/************************************************************************/

	//testMaterial->destroy();
#ifdef DX11
	gpuProgInclude.reset();
#endif

	gResources().unload(testTexRef);
	gResources().unload(dbgMeshRef);
	gResources().unload(fragProgRef);
	gResources().unload(vertProgRef);
	gResources().unload(testMaterial);

	testMaterial.reset();
	testTexRef.reset();
	dbgMeshRef.reset();
	fragProgRef.reset();
	vertProgRef.reset();

	testModelGO->destroy();

	newPassGL = nullptr;
	newTechniqueGL = nullptr;

	newPassDX = nullptr;
	newTechniqueDX = nullptr;

	newPassDX11 = nullptr;
	newTechniqueDX11 = nullptr;

	testShader = nullptr;
	
	renderWindow = nullptr;
	
	gBansheeApp().shutDown();

	return 0;
}

