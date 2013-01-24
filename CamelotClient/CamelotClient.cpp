// CamelotClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

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

int _tmain(int argc, _TCHAR* argv[])
{
	//gApplication().startUp("CamelotGLRenderSystem", "CamelotForwardRenderer");
	gApplication().startUp("CamelotD3D9RenderSystem", "CamelotForwardRenderer");
	//gApplication().startUp("CamelotD3D11RenderSystem", "CamelotForwardRenderer");

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

	HighLevelGpuProgramPtr fragProg;
	HighLevelGpuProgramPtr vertProg;

	/////////////////// HLSL 9 SHADERS //////////////////////////
	String fragShaderCode = "sampler2D tex;			\
							float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
							{														\
							float4 color = tex2D(tex, uv);				\
							return color;										\
							}";

	fragProg =  HighLevelGpuProgram::create(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);

	String vertShaderCode = "float4x4 matViewProjection;	\
							void vs_main(										\
							float4 inPos : POSITION,							\
							float2 uv : TEXCOORD0,								\
							out float4 oPosition : POSITION,					\
							out float2 oUv : TEXCOORD0)							\
							{														\
							oPosition = mul(matViewProjection, inPos);			\
							oUv = uv;											\
							}";

	vertProg =  HighLevelGpuProgram::create(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);

	/////////////////// HLSL 11 SHADERS //////////////////////////
	//String fragShaderCode = "SamplerState samp : register(s0);			\
	//						Texture2D tex : register(t0); \
	//						float4 ps_main(in float4 inPos : SV_Position, float2 uv : TEXCOORD0) : SV_Target		\
	//						{														\
	//						float4 color = tex.Sample(samp, uv);				\
	//						return color;										\
	//						}";

	//fragProg =  HighLevelGpuProgram::create(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_4_0);

	//String vertShaderCode = "float4x4 matViewProjection;	\
	//						void vs_main(										\
	//						in float4 inPos : POSITION,							\
	//						in float2 uv : TEXCOORD0,								\
	//						out float4 oPosition : SV_Position,					\
	//						out float2 oUv : TEXCOORD0)							\
	//						{														\
	//						oPosition = mul(matViewProjection, inPos);			\
	//						oUv = uv;											\
	//						}";

	//vertProg =  HighLevelGpuProgram::create(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_4_0);

	/////////////////// CG SHADERS //////////////////////////
	//String fragShaderCode = "sampler2D tex;					\
	//						float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
	//						{														\
	//						float4 color = tex2D(tex, uv);						\
	//						return color;										\
	//						}";

	//fragProg =  HighLevelGpuProgram::create(fragShaderCode, "ps_main", "cg", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);

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

	//vertProg =  HighLevelGpuProgram::create(vertShaderCode, "vs_main", "cg", GPT_VERTEX_PROGRAM, GPP_VS_2_0);

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

	//fragProg = HighLevelGpuProgram::create(fragShaderCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);

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

	//vertProg = HighLevelGpuProgram::create(vertShaderCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);

	HighLevelGpuProgramHandle vertProgRef(vertProg);

	gResources().create(vertProgRef, "C:\\vertProgCg.vprog", true);
	vertProgRef = static_resource_cast<HighLevelGpuProgram>(gResources().load("C:\\vertProgCg.vprog"));

	HighLevelGpuProgramHandle fragProgRef(fragProg);

	gResources().create(fragProgRef, "C:\\fragProgCg.vprog", true);
	fragProgRef = static_resource_cast<HighLevelGpuProgram>(gResources().load("C:\\fragProgCg.vprog"));

	ShaderPtr testShader = ShaderPtr(new Shader("TestShader"));
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

	MaterialHandle testMaterial = MaterialPtr(new Material());
	testMaterial->setShader(testShader);

	testMaterial->setMat4("matViewProjection", Matrix4::IDENTITY);

	//gResources().create(testMaterial, "C:\\testMaterial.mat", true);
	//testMaterial = static_resource_cast<MaterialHandle>(gResources().load("C:\\testMaterial.mat"));

	/*TextureRef testTex = static_resource_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));*/
	TextureHandle testTex = static_resource_cast<Texture>(Importer::instance().import("C:\\ArenaTowerDFS.psd"));
	MeshHandle dbgMesh = static_resource_cast<Mesh>(Importer::instance().import("C:\\X_Arena_Tower.FBX"));

	//int tmpFlag = _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_CRT_DF | _CRTDBG_DELAY_FREE_MEM_DF);

	gResources().create(testTex, "C:\\ExportTest.tex", true);
	gResources().create(dbgMesh, "C:\\ExportMesh.mesh", true);

	testTex = static_resource_cast<Texture>(gResources().load("C:\\ExportTest.tex"));
	dbgMesh = static_resource_cast<Mesh>(gResources().load("C:\\ExportMesh.mesh"));

	testMaterial->setTexture("tex", testTex);
	//gResources().create(testMaterial, "C:\\ExportMaterial.mat", true);

	//testMaterial = gResources().load("C:\\ExportMaterial.mat");

	//_ASSERT(_CrtCheckMemory());

	testRenderable->setMesh(dbgMesh);
	testRenderable->setMaterial(testMaterial);

	//// Set the new state for the flag
	//_CrtSetDbgFlag( tmpFlag );

	gApplication().runMainLoop();

	gApplication().shutDown();

	return 0;
}

