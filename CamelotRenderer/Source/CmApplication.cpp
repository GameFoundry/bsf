#include "CmApplication.h"

#include "CmRenderSystem.h"
#include "CmRenderSystemManager.h"

#include "CmWindowEventUtilities.h"
#include "CmHardwareBufferManager.h"
#include "CmRenderWindow.h"
#include "CmCamera.h"
#include "CmViewport.h"
#include "CmVector2.h"
#include "CmHighLevelGpuProgram.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmDynLib.h"
#include "CmDynLibManager.h"
#include "CmSceneManager.h"
#include "CmImporter.h"
#include "CmResources.h"
#include "CmMesh.h"
#include "CmGameObject.h"
#include "CmTime.h"
#include "CmInput.h"

#include "CmMaterial.h"
#include "CmShader.h"
#include "CmTechnique.h"
#include "CmPass.h"

#include "CmRendererManager.h"

namespace CamelotEngine
{
	Application::Application()
		:mRenderWindow(nullptr), mCamera(nullptr)
	{ }

	void Application::startUp(const String& renderSystemDll, const String& rendererDll)
	{
		Time::startUp(new Time());
		Input::startUp(new Input());
		DynLibManager::startUp(new DynLibManager());
		HighLevelGpuProgramManager::startUp(new HighLevelGpuProgramManager());

		RenderSystemManager::startUp(renderSystemDll);

		loadPlugin(rendererDll);
		RendererManager::setActive("ForwardRenderer");

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_initialise(false, "Camelot Renderer");

		SceneManager::startUp(new SceneManager());
		Resources::startUp(new Resources("D:\\CamelotResourceMetas"));

		mRenderWindow = renderSystem->_createRenderWindow("Camelot Renderer", 800, 600, false);

		//renderSystem->setAmbientLight(1.0f, 1.0f, 1.0f);
		renderSystem->setLightingEnabled(false);

		mCameraGO = GameObject::create("MainCamera");
		mCamera = mCameraGO->addComponent<Camera>();

		mCamera->init(mRenderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		mCameraGO->setPosition(Vector3(0,50,1240));
		mCameraGO->lookAt(Vector3(0,50,-300));
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(800.0f / 600.0f);

		/////////////////// HLSL SHADERS //////////////////////////
		//String fragShaderCode = "sampler2D tex;			\
		//						float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
		//						{														\
		//						float4 color = tex2D(tex, uv);				\
		//						return color;										\
		//						}";

		//mFragProg =  HighLevelGpuProgramManager::instance().createProgram(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		//mFragProg->load();

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

		//mVertProg =  HighLevelGpuProgramManager::instance().createProgram(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
		//mVertProg->load();
		


		/////////////////// CG SHADERS //////////////////////////
		String fragShaderCode = "sampler2D tex;					\
			float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
		{														\
			float4 color = tex2D(tex, uv);						\
			return color;										\
		}";

		mFragProg =  HighLevelGpuProgramManager::instance().createProgram(fragShaderCode, "ps_main", "cg", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		mFragProg->init();

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

		mVertProg =  HighLevelGpuProgramManager::instance().createProgram(vertShaderCode, "vs_main", "cg", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
		mVertProg->init();

		HighLevelGpuProgramRef vertProgRef(mVertProg);

		gResources().create(vertProgRef, "C:\\vertProgCg.vprog", true);
		vertProgRef = static_resource_cast<HighLevelGpuProgram>(gResources().load("C:\\vertProgCg.vprog"));

		HighLevelGpuProgramRef fragProgRef(mFragProg);

		gResources().create(fragProgRef, "C:\\fragProgCg.vprog", true);
		fragProgRef = static_resource_cast<HighLevelGpuProgram>(gResources().load("C:\\fragProgCg.vprog"));

		///////////////// GLSL SHADERS ////////////////////////////
		//String fragShaderCode = "uniform sampler2D tex; \
		//							void main() \
		//							  {\
		//							  vec4 texColor = texture2D(tex,gl_TexCoord[0].st);\
		//							  gl_FragColor = texColor; \
		//							  }";

		//mFragProg = HighLevelGpuProgramManager::instance().createProgram(fragShaderCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		//mFragProg->load();

		//// TODO - Ogres GLSL parsing requires some strict parameter naming, can that be avoided?
		//String vertShaderCode = "uniform mat4 matViewProjection; \
		//							  attribute vec4 vertex; \
		//						void main() \
		//							  { \
		//							  gl_TexCoord[0] = gl_MultiTexCoord0; \
		//							  gl_Position = matViewProjection * vertex; \
		//							  }";

		//mVertProg = HighLevelGpuProgramManager::instance().createProgram(vertShaderCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
		//mVertProg->load();

		mTestShader = ShaderPtr(new Shader("TestShader"));
		TechniquePtr newTechnique = mTestShader->addTechnique("GLRenderSystem", "ForwardRenderer");
		PassPtr newPass = newTechnique->addPass();
		newPass->setVertexProgram(vertProgRef);
		newPass->setFragmentProgram(fragProgRef);

		mTestMaterial = MaterialPtr(new Material());
		mTestMaterial->setShader(mTestShader);

		// IMPORTER TEST
		Importer::startUp(new Importer());
		loadPlugin("CamelotFreeImgImporter"); // TODO - Load this automatically somehow
		loadPlugin("CamelotFBXImporter"); // TODO - Load this automatically somehow

		//mDbgTexture = std::static_pointer_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));
		TextureRef testTex = static_resource_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));
		mDbgMesh = static_resource_cast<Mesh>(Importer::instance().import("C:\\X_Arena_Tower.FBX"));
		//mDbgMesh = std::static_pointer_cast<Mesh>(Importer::instance().import("C:\\BarrelMesh.fbx"));

		gResources().create(testTex, "C:\\ExportTest.tex", true);
		gResources().create(mDbgMesh, "C:\\ExportMesh.mesh", true);

		mDbgTexture = static_resource_cast<Texture>(gResources().load("C:\\ExportTest.tex"));
		mDbgMesh = static_resource_cast<Mesh>(gResources().load("C:\\ExportMesh.mesh"));

		mDbgTexture = testTex;

		loadPlugin("CamelotOISInput"); // TODO - Load this automatically somehow
	}

	void Application::runMainLoop()
	{
		while(true)
		{
			WindowEventUtilities::messagePump();

			DBG_renderSimpleFrame();

			gTime().update();
			gInput().update();
		}
	}

	void Application::shutDown()
	{
		SceneManager::shutDown();

		if(RenderSystemManager::getActive() != nullptr)
			RenderSystemManager::getActive()->shutdown();

		HighLevelGpuProgramManager::shutDown();
		DynLibManager::shutDown();
		Resources::shutDown();
		Input::shutDown();
		Time::shutDown();
	}

	void Application::loadPlugin(const String& pluginName)
	{
		String name = pluginName;
#if CM_PLATFORM == CM_PLATFORM_LINUX
		// dlopen() does not add .so to the filename, like windows does for .dll
		if (name.substr(name.length() - 3, 3) != ".so")
			name += ".so";
#elif CM_PLATFORM == CM_PLATFORM_APPLE
		// dlopen() does not add .dylib to the filename, like windows does for .dll
		if (name.substr(name.length() - 6, 6) != ".dylib")
			name += ".dylib";
#elif CM_PLATFORM == CM_PLATFORM_WIN32
		// Although LoadLibraryEx will add .dll itself when you only specify the library name,
		// if you include a relative path then it does not. So, add it to be sure.
		if (name.substr(name.length() - 4, 4) != ".dll")
			name += ".dll";
#endif

		DynLib* library = gDynLibManager().load(name);

		if(library != nullptr)
		{
			typedef const void (*LoadPluginFunc)();

			LoadPluginFunc loadPluginFunc = (LoadPluginFunc)library->getSymbol("loadPlugin");
			loadPluginFunc();
		}
	}

	UINT32 Application::getAppWindowId()
	{
		if(!mRenderWindow)
		{
			CM_EXCEPT(InternalErrorException, "Unable to get window handle. No active window is set!");
		}

		UINT32 windowId;
		mRenderWindow->getCustomAttribute("WINDOW", &windowId);

		return windowId;
	}

	void Application::DBG_renderSimpleFrame()
	{
		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_setViewport(mCamera->getViewport());

		//Matrix4 projMatrix = mCamera->getProjectionMatrixRS();
		//renderSystem->_setProjectionMatrix(projMatrix);

		//Matrix4 viewMatrix = mCamera->getViewMatrix(true);
		//renderSystem->_setViewMatrix(viewMatrix);

		Matrix4 projMatrixCstm = mCamera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = mCamera->getViewMatrix();

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderSystem->setInvertVertexWinding(false);
		renderSystem->_setDepthBufferParams();
		renderSystem->clearFrameBuffer(FBT_COLOUR | FBT_DEPTH, Color::Blue);
		renderSystem->_beginFrame();

		mTestMaterial->setMat4("matViewProjection", viewProjMatrix);
		mTestMaterial->setTexture("tex", mDbgTexture);

		mTestMaterial->applyPass(0);
		
		renderSystem->_render(mDbgMesh->getRenderOperation());

		renderSystem->_endFrame();

		renderSystem->_swapAllRenderTargetBuffers(false);
	}

	Application& gApplication()
	{
		static Application application;
		return application;
	}
}