// CamelotClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>

#include "CmApplication.h"
#include "CmDynLibManager.h"

#include "CmGameObject.h"
#include "CmCamera.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmRenderSystem.h"
#include "CmRenderWindow.h"
#include "CmResources.h"
#include "CmRenderable.h"
#include "CmMaterial.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"
#include "CmImporter.h"
#include "CmMesh.h"

#include "CmDebugCamera.h"

using namespace CamelotEngine;

int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	//gApplication().startUp("CamelotGLRenderSystem", "CamelotForwardRenderer");
	//gApplication().startUp("CamelotD3D9RenderSystem", "CamelotForwardRenderer");
	gApplication().startUp("CamelotD3D11RenderSystem", "CamelotForwardRenderer");

	RenderSystem* renderSystem = RenderSystem::instancePtr();
	RenderWindowPtr renderWindow = gApplication().getPrimaryRenderWindow();

	GameObjectPtr cameraGO = GameObject::create("MainCamera");
	CameraPtr camera = cameraGO->addComponent<Camera>();

	camera->init(renderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
	cameraGO->setPosition(Vector3(0,50,1240));
	cameraGO->lookAt(Vector3(0,50,-300));
	camera->setNearClipDistance(5);
	camera->setAspectRatio(800.0f / 600.0f);

	std::shared_ptr<DebugCamera> debugCamera = cameraGO->addComponent<DebugCamera>();

	GameObjectPtr testModelGO = GameObject::create("TestMesh");
	RenderablePtr testRenderable = testModelGO->addComponent<Renderable>();

	/////////////////// HLSL 9 SHADERS //////////////////////////
	//String fragShaderCode = "sampler2D tex;			\
	//						float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
	//						{														\
	//						float4 color = tex2D(tex, uv);				\
	//						return color;										\
	//						}";

	//HighLevelGpuProgramHandle fragProgRef =  HighLevelGpuProgram::create(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);

	//String vertShaderCode = "float4x4 matViewProjection;	\
	//						void vs_main(										\
	//						float4 inPos : POSITION,							\
	//						float2 uv : TEXCOORD0,								\
	//						out float4 oPosition : POSITION,					\
	//						out float2 oUv : TEXCOORD0)							\
	//						{														\
	//						oPosition = mul(matViewProjection, inPos);			\
	//						oUv = uv;											\
	//						}";

	//HighLevelGpuProgramHandle vertProgRef =  HighLevelGpuProgram::create(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);

	/////////////////// HLSL 11 SHADERS //////////////////////////
	String fragShaderCode = "SamplerState samp : register(s0);			\
							Texture2D tex : register(t0); \
							float4 ps_main(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target		\
							{														\
							float4 color = tex.Sample(samp, uv);				\
							return color;										\
							}";

	HighLevelGpuProgramHandle fragProgRef =  HighLevelGpuProgram::create(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

	String vertShaderCode = "float4x4 matViewProjection;	\
							void vs_main(										\
							in float4 inPos : POSITION,							\
							in float2 uv : TEXCOORD0,								\
							out float4 oPosition : SV_Position,					\
							out float2 oUv : TEXCOORD0)							\
							{														\
							oPosition = mul(matViewProjection, inPos);			\
							oUv = uv;											\
							}";

	HighLevelGpuProgramHandle vertProgRef =  HighLevelGpuProgram::create(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);

	/////////////////// CG SHADERS //////////////////////////
	//String fragShaderCode = "sampler2D tex;					\
	//						float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
	//						{														\
	//						float4 color = tex2D(tex, uv);						\
	//						return color;										\
	//						}";

	//HighLevelGpuProgramHandle fragProgRef =  HighLevelGpuProgram::create(fragShaderCode, "ps_main", "cg", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);

	//String vertShaderCode = "float4x4 matViewProjection;	\
	//						void vs_main(										\
	//						float4 cm_position : POSITION,							\
	//						float2 cm_texcoord0 : TEXCOORD0,								\
	//						out float4 oPosition : POSITION,					\
	//						out float2 oUv : TEXCOORD0)							\
	//						{														\
	//						oPosition = mul(matViewProjection, cm_position);			\
	//						oUv = cm_texcoord0;											\
	//						}";

	//HighLevelGpuProgramHandle vertProgRef =  HighLevelGpuProgram::create(vertShaderCode, "vs_main", "cg", GPT_VERTEX_PROGRAM, GPP_VS_2_0);

	///////////////// GLSL SHADERS ////////////////////////////
	//String fragShaderCode = " #version 400 \n \
	//						  uniform sampler2D tex; \
	//						  in vec2 texcoord0; \
	//						  out vec4 fragColor; \
	//						  void main() \
	//						  {\
	//							  vec4 texColor = texture2D(tex, texcoord0.st);\
	//							  fragColor = texColor; \
	//						  }";

	//HighLevelGpuProgramHandle fragProgRef = HighLevelGpuProgram::create(fragShaderCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);

	//// TODO - Make sure to document the strict input parameter naming. (Exact supported names are in GLSLParamParser)
	//String vertShaderCode = "#version 400 \n \
	//						 uniform mainFragBlock { mat4 matViewProjection; }; \
	//						 in vec4 cm_position; \
	//						 in vec2 cm_texcoord0; \
	//						 out vec2 texcoord0; \
	//						 void main() \
	//						 { \
	//							texcoord0 = cm_texcoord0; \
	//							gl_Position = cm_position * matViewProjection; \
	//						 }";

	//HighLevelGpuProgramHandle vertProgRef= HighLevelGpuProgram::create(vertShaderCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);

	gResources().create(vertProgRef, "C:\\vertProgCg.vprog", true);
	gResources().unload(vertProgRef);
	vertProgRef = gResources().load("C:\\vertProgCg.vprog");

	gResources().create(fragProgRef, "C:\\fragProgCg.vprog", true);
	gResources().unload(fragProgRef);
	fragProgRef = gResources().load("C:\\fragProgCg.vprog");

	ShaderPtr testShader = Shader::create("TestShader");
	testShader->addParameter("matViewProjection", "matViewProjection", GPDT_MATRIX_4X4);
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

	MaterialHandle testMaterial = Material::create();
	testMaterial.waitUntilLoaded(); // TODO - Material doesn't do anything GPU specific, so technically it should be possible to initialize on the spot
	// but is that a good idea?
	testMaterial->setShader(testShader);

	testMaterial->setMat4("matViewProjection", Matrix4::IDENTITY);
	//testMaterialRef = gResources().load("C:\\testMaterial.mat");
	//testMaterialRef.waitUntilLoaded();

	/*TextureRef testTex = static_resource_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));*/
	TextureHandle testTexRef = static_resource_cast<Texture>(Importer::instance().import("C:\\ArenaTowerDFS.psd"));
	MeshHandle dbgMeshRef = static_resource_cast<Mesh>(Importer::instance().import("C:\\X_Arena_Tower.FBX"));

	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_DELAY_FREE_MEM_DF);

	gResources().create(testTexRef, "C:\\ExportTest.tex", true);
	gResources().create(dbgMeshRef, "C:\\ExportMesh.mesh", true);

	gResources().unload(testTexRef);
	gResources().unload(dbgMeshRef);

	testTexRef = static_resource_cast<Texture>(gResources().load("C:\\ExportTest.tex"));
	dbgMeshRef = static_resource_cast<Mesh>(gResources().load("C:\\ExportMesh.mesh"));

	testMaterial->setTexture("tex", testTexRef);
	//gResources().create(testMaterial, "C:\\ExportMaterial.mat", true);

	//testMaterial = gResources().load("C:\\ExportMaterial.mat");

	//_ASSERT(_CrtCheckMemory());

	//dbgMeshRef.waitUntilLoaded();

	testRenderable->setMesh(dbgMeshRef);
	testRenderable->setMaterial(testMaterial);

	//// Set the new state for the flag
	//_CrtSetDbgFlag( tmpFlag );

	gApplication().runMainLoop();

	// Release everything before shutdown
	
	//testMaterial->destroy();
	testMaterial.reset();

	gResources().unload(testTexRef);
	gResources().unload(dbgMeshRef);
	gResources().unload(fragProgRef);
	gResources().unload(vertProgRef);

	testTexRef.reset();
	dbgMeshRef.reset();
	fragProgRef.reset();
	vertProgRef.reset();

	testModelGO->destroy();
	testModelGO = nullptr;
	testRenderable = nullptr;

	cameraGO->destroy();
	cameraGO = nullptr;
	camera = nullptr;
	debugCamera = nullptr;

	newPassGL = nullptr;
	newTechniqueGL = nullptr;

	newPassDX = nullptr;
	newTechniqueDX = nullptr;

	newPassDX11 = nullptr;
	newTechniqueDX11 = nullptr;

	testShader = nullptr;
	
	renderWindow = nullptr;

	gApplication().shutDown();

	return 0;
}

