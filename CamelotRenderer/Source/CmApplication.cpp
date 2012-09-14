#include "CmApplication.h"

#include "CmRenderSystem.h"
#include "CmRenderSystemManager.h"

#include "CmWindowEventUtilities.h"
#include "CmHardwareBufferManager.h"
#include "CmRenderWindow.h"
#include "CmCamera.h"
#include "CmViewport.h"
#include "CmHighLevelGpuProgram.h"
#include "CmHighLevelGpuProgramManager.h"
#include "CmDynLibManager.h"

namespace CamelotEngine
{
	Application::Application()
		:mRenderWindow(nullptr), mViewport(nullptr), mCamera(nullptr)
	{ }

	void Application::startUp(String renderSystemDll)
	{
		DynLibManager::startUp(new DynLibManager());
		HighLevelGpuProgramManager::startUp(new HighLevelGpuProgramManager());

		RenderSystemManager::initialize(renderSystemDll);

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_initialise(false, "Camelot Renderer");

		mRenderWindow = renderSystem->_createRenderWindow("Camelot Renderer", 800, 600, false);

		//renderSystem->setAmbientLight(1.0f, 1.0f, 1.0f);
		renderSystem->setLightingEnabled(false);

		mCamera = new Camera("SimpleCam");
		mCamera->setPosition(Vector3(0,0,80));
		mCamera->lookAt(Vector3(0,0,-300));
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(480.0f / 640.0f);

		mViewport = mRenderWindow->addViewport();

		/////////////////// HLSL SHADERS //////////////////////////

		//String fragShaderCode = "float4 ps_main() : COLOR0	\
		//{														\
		//	float4 color = float4(0, 0, 0, 0);					\
		//	color.r = 1.0f;										\
		//	color.a = 1.0f;										\
		//	return color;										\
		//}";

		//mFragProg = mGpuProgramManager->createProgram(fragShaderCode, "ps_main", "hlsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		//mFragProg->load();

		//String vertShaderCode = "float4x4 matViewProjection; \
		//float4 vs_main(float4 inPos : POSITION) : POSITION \
		//{ \
		//	return mul(matViewProjection, inPos); \
		//}";

		//mVertProg = mGpuProgramManager->createProgram(vertShaderCode, "vs_main", "hlsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
		//mVertProg->load();

		///////////////// GLSL SHADERS ////////////////////////////
		String fragShaderCode = "void main() \
									  {\
									  gl_FragColor = vec4(0.0,1.0,0.0,1.0); \
									  }";

		mFragProg = HighLevelGpuProgramManager::instance().createProgram(fragShaderCode, "main", "glsl", GPT_FRAGMENT_PROGRAM, GPP_PS_2_0);
		mFragProg->load();

		// TODO - Ogres GLSL parsing requires some strict parameter naming, can that be avoided?
		String vertShaderCode = "uniform mat4 matViewProjection; \
									  attribute vec4 vertex; \
								void main() \
									  { \
									  gl_Position = matViewProjection * vertex; \
									  }";

		mVertProg = HighLevelGpuProgramManager::instance().createProgram(vertShaderCode, "main", "glsl", GPT_VERTEX_PROGRAM, GPP_VS_2_0);
		mVertProg->load();


		while(true)
		{
			WindowEventUtilities::messagePump();

			DBG_renderSimpleFrame();
		}
	}

	void Application::shutDown()
	{
		if(RenderSystemManager::getActive() != nullptr)
			RenderSystemManager::getActive()->shutdown();

		HighLevelGpuProgramManager::shutDown();
		DynLibManager::shutDown();
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

		//decl->addElement(0, offset, VET_COLOUR, VES_DIFFUSE);
		//offset += VertexElement::getTypeSize(VET_COLOUR);

		HardwareVertexBufferPtr vertexBuffer = HardwareBufferManager::instance().createVertexBuffer(
			vertexData->vertexDeclaration->getVertexSize(0),
			vertexData->vertexCount,
			HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

		size_t vertexSize = vertexBuffer->getVertexSize();
		char* vertBufferData = static_cast<char*>(vertexBuffer->lock(HardwareBuffer::HBL_NORMAL));

		Vector3 position(-5.0f, -5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(-5.0f, 5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(5.0f, 5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(5.0f, -5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(-5.0f, -5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(5.0f, -5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(5.0f, 5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Vector3(-5.0f, 5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		vertexBuffer->unlock();

		ro.indexData = indexData;
		ro.vertexData = vertexData;
		ro.useIndexes = true;
		ro.operationType = RenderOperation::OT_TRIANGLE_LIST;

		RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_setViewport(mViewport);

		//Matrix4 projMatrix = mCamera->getProjectionMatrixRS();
		//renderSystem->_setProjectionMatrix(projMatrix);

		//Matrix4 viewMatrix = mCamera->getViewMatrix(true);
		//renderSystem->_setViewMatrix(viewMatrix);

		Matrix4 projMatrixCstm = mCamera->getProjectionMatrix();
		Matrix4 viewMatrixCstm = mCamera->getViewMatrix(true);

		Matrix4 viewProjMatrix = projMatrixCstm * viewMatrixCstm;

		renderSystem->setInvertVertexWinding(true);
		renderSystem->clearFrameBuffer(FBT_COLOUR | FBT_DEPTH, Color::Blue);
		renderSystem->_beginFrame();

		mVertProg->getDefaultParameters()->setNamedConstant("matViewProjection", viewProjMatrix);

		//renderSystem->bindGpuProgramParameters(GPT_VERTEX_PROGRAM, mVertProg->getDefaultParameters(), GPV_ALL);

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