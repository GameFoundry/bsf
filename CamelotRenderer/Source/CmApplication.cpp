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

namespace CamelotEngine
{
	Application::Application()
		:mRenderWindow(nullptr), mCamera(nullptr)
	{ }

	void Application::startUp(String renderSystemDll)
	{
		DynLibManager::startUp(new DynLibManager());
		HighLevelGpuProgramManager::startUp(new HighLevelGpuProgramManager());

		RenderSystemManager::initialize(renderSystemDll);

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_initialise(false, "Camelot Renderer");

		SceneManager::startUp(new SceneManager());

		mRenderWindow = renderSystem->_createRenderWindow("Camelot Renderer", 800, 600, false);

		//renderSystem->setAmbientLight(1.0f, 1.0f, 1.0f);
		renderSystem->setLightingEnabled(false);

		mCamera = new Camera(mRenderWindow, 0.0f, 0.0f, 1.0f, 1.0f, 0);
		mCamera->setPosition(Vector3(0,0,40));
		mCamera->lookAt(Vector3(0,0,-300));
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(600.0f / 800.0f);

		/////////////////// HLSL SHADERS //////////////////////////
		//String fragShaderCode = "sampler2D diffuseMap;			\
		//	float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
		//{														\
		//	float4 color = tex2D(diffuseMap, uv);				\
		//	return color;										\
		//}";

		//mFragProg =  HighLevelGpuProgramManager::instance().createProgram(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		//mFragProg->load();

		//String vertShaderCode = "float4x4 matViewProjection;	\
		//	void vs_main(										\
		//	float4 inPos : POSITION,							\
		//	float2 uv : TEXCOORD0,								\
		//	out float4 oPosition : POSITION,					\
		//	out float2 oUv : TEXCOORD0)							\
		//{														\
		//	oPosition = mul(matViewProjection, inPos);			\
		//	oUv = uv;											\
		//}";

		//mVertProg =  HighLevelGpuProgramManager::instance().createProgram(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
		//mVertProg->load();
		


		/////////////////// CG SHADERS //////////////////////////
		String fragShaderCode = "sampler2D diffuseMap;			\
			float4 ps_main(float2 uv : TEXCOORD0) : COLOR0		\
		{														\
			float4 color = tex2D(diffuseMap, uv);				\
			return color;										\
		}";

		mFragProg =  HighLevelGpuProgramManager::instance().createProgram(fragShaderCode, "ps_main", "cg", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		mFragProg->load();

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
		mVertProg->load();
		


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


		// IMPORTER TEST
		Importer::startUp(new Importer());
		DynLib* loadedLibrary = gDynLibManager().load("CamelotFreeImgImporter.dll"); // TODO - Load this automatically somehow

		if(loadedLibrary != nullptr)
		{
			typedef const void (*LoadPluginFunc)();

			LoadPluginFunc loadPluginFunc = (LoadPluginFunc)loadedLibrary->getSymbol("loadPlugin");
			loadPluginFunc();
		}

		//mDbgTexture = std::static_pointer_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));
		TexturePtr testTex = std::static_pointer_cast<Texture>(Importer::instance().import("C:\\ImportTest.tga"));
		Resources::startUp(new Resources());

		gResources().save(testTex, "C:\\ExportTest.tex");
		mDbgTexture = std::static_pointer_cast<Texture>(gResources().load("C:\\ExportTest.tex"));
		mDbgTexture = testTex;
	}

	void Application::runMainLoop()
	{
		while(true)
		{
			WindowEventUtilities::messagePump();

			DBG_renderSimpleFrame();
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
	}

	void Application::DBG_renderSimpleFrame()
	{
		RenderOperation ro;
		IndexData* indexData = new IndexData();

		indexData->indexCount = 36;
		indexData->indexBuffer = HardwareBufferManager::instance().createIndexBuffer(
			HardwareIndexBuffer::IT_16BIT,
			36, 
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		unsigned short* idxData = static_cast<unsigned short*>(indexData->indexBuffer->lock(HardwareBuffer::HBL_NORMAL));

		idxData[0] = 0; idxData[1] = 1; idxData[2] = 2;
		idxData[3] = 2; idxData[4] = 3; idxData[5] = 0;

		idxData[6] = 4; idxData[7] = 5; idxData[8] = 6;
		idxData[9] = 6; idxData[10] = 7; idxData[11] = 4;

		idxData[12] = 0; idxData[13] = 3; idxData[14] = 5;
		idxData[15] = 5; idxData[16] = 4; idxData[17] = 0;

		idxData[18] = 3; idxData[19] = 2; idxData[20] = 6;
		idxData[21] = 6; idxData[22] = 5; idxData[23] = 3;

		idxData[24] = 2; idxData[25] = 1; idxData[26] = 7;
		idxData[27] = 7; idxData[28] = 6; idxData[29] = 2;

		idxData[30] = 1; idxData[31] = 0; idxData[32] = 4;
		idxData[33] = 4; idxData[34] = 7; idxData[35] = 1;

		indexData->indexBuffer->unlock();


		VertexData* vertexData = new VertexData();

		vertexData->vertexStart = 0;
		vertexData->vertexCount = 8;

		VertexDeclaration* decl = vertexData->vertexDeclaration;
		decl->removeAllElements();

		size_t offset = 0;
		decl->addElement(0, offset, VET_FLOAT3, VES_POSITION);
		offset += VertexElement::getTypeSize(VET_FLOAT3);
		decl->addElement(0, offset, VET_FLOAT2, VES_TEXTURE_COORDINATES);
		offset += VertexElement::getTypeSize(VET_FLOAT2);

		//decl->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
		//offset += VertexElement::getTypeSize(VET_COLOUR);

		HardwareVertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
			vertexData->vertexDeclaration->getVertexSize(0),
			vertexData->vertexCount,
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

		size_t vertexSize = vertexBuffer->getVertexSize();
		char* vertBufferData = static_cast<char*>(vertexBuffer->lock(HardwareBuffer::HBL_NORMAL));

		size_t posSize = VertexElement::getTypeSize(VET_FLOAT3);
		size_t uvSize = VertexElement::getTypeSize(VET_FLOAT2);

		Vector3 position(-5.0f, -5.0f, -5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		Vector2 uv(0.0f, 0.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(-5.0f, 5.0f, -5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(0.0f, 1.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(5.0f, 5.0f, -5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(1.0f, 1.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(5.0f, -5.0f, -5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(1.0f, 0.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(-5.0f, -5.0f, 5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(0.0f, 0.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(5.0f, -5.0f, 5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(1.0f, 0.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(5.0f, 5.0f, 5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(1.0f, 1.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;


		position = 	Vector3(-5.0f, 5.0f, 5.0f);
		memcpy(vertBufferData, &position, posSize);
		vertBufferData += posSize;

		uv = Vector2(0.0f, 1.0f);
		memcpy(vertBufferData, &uv, uvSize);
		vertBufferData += uvSize;

		vertexBuffer->unlock();

		ro.indexData = indexData;
		ro.vertexData = vertexData;
		ro.useIndexes = true;
		ro.operationType = RenderOperation::OT_TRIANGLE_LIST;

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

		mVertProg->getDefaultParameters()->setNamedConstant("matViewProjection", viewProjMatrix);

		//renderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, mVertProg->getDefaultParameters(), GPV_ALL);

		renderSystem->_setTexture(0, true, mDbgTexture);

		renderSystem->bindGpuProgram(mFragProg->_getBindingDelegate()); // TODO - I don't like this. Shader should be able to be bound directly!
		renderSystem->bindGpuProgram(mVertProg->_getBindingDelegate()); // TODO - I don't like this. Shader should be able to be bound directly!

		// TODO - Shaders need to be bound and only then parameters can be set. I need to encapuslate this better because I can't expect users to know that
		renderSystem->bindGpuProgramParameters(GPT_FRAGMENT_PROGRAM, mFragProg->getDefaultParameters(), GPV_ALL); // TODO - If I dont call bind parameters before shader wont activate? I think I should handle that differently
		renderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, mVertProg->getDefaultParameters(), GPV_ALL);

		
		
		renderSystem->_render(ro);

		renderSystem->_endFrame();

		renderSystem->_swapAllRenderTargetBuffers(false);
	}

	Application& gApplication()
	{
		static Application application;
		return application;
	}
}