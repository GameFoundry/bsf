#include "CmApplication.h"

#include "OgreRenderSystem.h"
#include "CmRenderSystemManager.h"

#include "OgreWindowEventUtilities.h"
#include "OgreHardwareBufferManager.h"
#include "OgreRenderWindow.h"
#include "OgreCamera.h"
#include "OgreViewport.h"
#include "OgreHighLevelGpuProgram.h"
#include "OgreHighLevelGpuProgramManager.h"

namespace CamelotEngine
{
	Application::Application()
		:mRenderWindow(nullptr), mViewport(nullptr), mCamera(nullptr), mGpuProgramManager(nullptr)
	{ }

	void Application::startUp()
	{
		mGpuProgramManager = new Ogre::HighLevelGpuProgramManager(); // TODO - Use Camelot::Module for instantiating this

		RenderSystemManager::initialize("D3D9RenderSystem");
		
		Ogre::RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_initialise(false, "Camelot Renderer");

		mRenderWindow = renderSystem->_createRenderWindow("Camelot Renderer", 800, 600, false);

		//renderSystem->setAmbientLight(1.0f, 1.0f, 1.0f);
		renderSystem->setLightingEnabled(false);

		mCamera = new Ogre::Camera("SimpleCam", nullptr);
		mCamera->setPosition(Ogre::Vector3(0,0,80));
		mCamera->lookAt(Ogre::Vector3(0,0,-300));
		mCamera->setNearClipDistance(5);
		mCamera->setAspectRatio(480.0f / 640.0f);

		mViewport = mRenderWindow->addViewport();

		Ogre::String fragShaderCode = "float4 ps_main() : COLOR0	\
		{														\
			float4 color = float4(0, 0, 0, 0);					\
			color.r = 1.0f;										\
			color.a = 1.0f;										\
			return color;										\
		}";

		mFragProg = mGpuProgramManager->createProgram(fragShaderCode, "ps_main", "hlsl", Ogre::GPT_FRAGMENT_PROGRAM, Ogre::GPP_PS_2_0);
		mFragProg->load();

		Ogre::String vertShaderCode = "float4x4 matViewProjection; \
		float4 vs_main(float4 inPos : POSITION) : POSITION \
		{ \
			return mul(matViewProjection, inPos); \
		}";

		mVertProg = mGpuProgramManager->createProgram(vertShaderCode, "vs_main", "hlsl", Ogre::GPT_VERTEX_PROGRAM, Ogre::GPP_VS_2_0);
		mVertProg->load();

		while(true)
		{
			Ogre::WindowEventUtilities::messagePump();

			DBG_renderSimpleFrame();
		}
	}

	void Application::shutDown()
	{
		if(RenderSystemManager::getActive() != nullptr)
			RenderSystemManager::getActive()->shutdown();

		if(mGpuProgramManager != nullptr)
			delete mGpuProgramManager;
	}

	void Application::DBG_renderSimpleFrame()
	{
		Ogre::RenderOperation ro;
		Ogre::IndexData* indexData = new Ogre::IndexData();

		indexData->indexCount = 36;
		indexData->indexBuffer = Ogre::HardwareBufferManager::getSingleton().createIndexBuffer(
			Ogre::HardwareIndexBuffer::IT_16BIT,
			36, 
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		unsigned short* idxData = static_cast<unsigned short*>(indexData->indexBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL));

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


		Ogre::VertexData* vertexData = new Ogre::VertexData();

		vertexData->vertexStart = 0;
		vertexData->vertexCount = 8;

		Ogre::VertexDeclaration* decl = vertexData->vertexDeclaration;
		decl->removeAllElements();

		size_t offset = 0;
		decl->addElement(0, offset, Ogre::VET_FLOAT3, Ogre::VES_POSITION);
		offset += Ogre::VertexElement::getTypeSize(Ogre::VET_FLOAT3);

		//decl->addElement(0, offset, Ogre::VET_COLOUR, Ogre::VES_DIFFUSE);
		//offset += Ogre::VertexElement::getTypeSize(Ogre::VET_COLOUR);

		Ogre::HardwareVertexBufferSharedPtr vertexBuffer = Ogre::HardwareBufferManager::getSingleton().createVertexBuffer(
			vertexData->vertexDeclaration->getVertexSize(0),
			vertexData->vertexCount,
			Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		vertexData->vertexBufferBinding->setBinding(0, vertexBuffer);

		size_t vertexSize = vertexBuffer->getVertexSize();
		char* vertBufferData = static_cast<char*>(vertexBuffer->lock(Ogre::HardwareBuffer::HBL_NORMAL));

		Ogre::Vector3 position(-5.0f, -5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(-5.0f, 5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(5.0f, 5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(5.0f, -5.0f, -5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(-5.0f, -5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(5.0f, -5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(5.0f, 5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		position = 	Ogre::Vector3(-5.0f, 5.0f, 5.0f);
		memcpy(vertBufferData, &position, vertexSize);
		vertBufferData += vertexSize;

		vertexBuffer->unlock();

		ro.indexData = indexData;
		ro.vertexData = vertexData;
		ro.useIndexes = true;
		ro.operationType = Ogre::RenderOperation::OT_TRIANGLE_LIST;

		Ogre::RenderSystem* renderSystem = RenderSystemManager::getActive();
		renderSystem->_setViewport(mViewport);

		//Ogre::Matrix4 projMatrix = mCamera->getProjectionMatrixRS();
		//renderSystem->_setProjectionMatrix(projMatrix);

		//Ogre::Matrix4 viewMatrix = mCamera->getViewMatrix(true);
		//renderSystem->_setViewMatrix(viewMatrix);

		Ogre::Matrix4 projMatrix = mCamera->getProjectionMatrix();
		Ogre::Matrix4 viewMatrix = mCamera->getViewMatrix(true);

		Ogre::Matrix4 viewProjMatrix = projMatrix * viewMatrix;

		renderSystem->setInvertVertexWinding(true);
		renderSystem->clearFrameBuffer(Ogre::FBT_COLOUR | Ogre::FBT_DEPTH, Ogre::ColourValue::Blue);
		renderSystem->_beginFrame();

		mVertProg->getDefaultParameters()->setNamedConstant("matViewProjection", viewProjMatrix);

		renderSystem->bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM, mVertProg->getDefaultParameters(), Ogre::GPV_ALL);
		renderSystem->bindGpuProgram(mVertProg->_getBindingDelegate()); // TODO - I don't like this. Shader should be able to be bound directly!
		renderSystem->bindGpuProgramParameters(Ogre::GPT_VERTEX_PROGRAM, mVertProg->getDefaultParameters(), Ogre::GPV_ALL);

		renderSystem->bindGpuProgram(mFragProg->_getBindingDelegate()); // TODO - I don't like this. Shader should be able to be bound directly!
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