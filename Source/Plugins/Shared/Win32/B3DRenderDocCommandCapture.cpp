//************************************* B3D Framework - Copyright 2026 Marko Pintera *************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "B3DRenderDocCommandCapture.h"
#include <RenderDoc/renderdoc_app.h>

#define WIN32_LEAN_AND_MEAN
#if !defined(NOMINMAX) && defined(_MSC_VER)
#	define NOMINMAX // required to stop windows.h messing up std::min
#endif
#include <windows.h>

using namespace b3d;

RenderDocCommandCapture::RenderDocCommandCapture(VkInstance vulkanInstance) :
	mVulkanInstance(vulkanInstance)
{
	// RenderDoc can only capture graphics API objects created after its library was loaded, so only a library it injected before
	// start-up is used. Loading the library here can also deadlock with the Vulkan driver's threads on the loader lock.
	const HMODULE renderDocModule = GetModuleHandleA("renderdoc.dll");
	if (renderDocModule == nullptr)
		return;

	const pRENDERDOC_GetAPI fnGetAPI = reinterpret_cast<pRENDERDOC_GetAPI>(GetProcAddress(renderDocModule, "RENDERDOC_GetAPI"));
	if (!B3D_ENSURE(fnGetAPI))
		return;

	if (!B3D_ENSURE(fnGetAPI(eRENDERDOC_API_Version_1_0_0, &mRenderDocAPIPointers) == 1))
		mRenderDocAPIPointers = nullptr;
}

void RenderDocCommandCapture::Start()
{
	if (!mRenderDocAPIPointers)
		return;

	RENDERDOC_API_1_0_0* renderDocAPI = static_cast<RENDERDOC_API_1_0_0*>(mRenderDocAPIPointers);
	renderDocAPI->StartFrameCapture(RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE(mVulkanInstance), nullptr);

	mIsCaptureInProgress = true;
}

void RenderDocCommandCapture::Stop()
{
	if (!mIsCaptureInProgress)
		return;

	if (mRenderDocAPIPointers)
	{
		RENDERDOC_API_1_0_0* renderDocAPI = static_cast<RENDERDOC_API_1_0_0*>(mRenderDocAPIPointers);
		B3D_ENSURE(renderDocAPI->EndFrameCapture(RENDERDOC_DEVICEPOINTER_FROM_VKINSTANCE(mVulkanInstance), nullptr) == 1);

		renderDocAPI->ShowReplayUI();
	}

	mIsCaptureInProgress = false;
}

