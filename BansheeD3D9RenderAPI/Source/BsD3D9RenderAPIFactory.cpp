//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsD3D9RenderAPIFactory.h"
#include "BsRenderAPI.h"

namespace BansheeEngine
{
	void D3D9RenderAPIFactory::create()
	{
	#ifdef BS_STATIC_LIB
		HINSTANCE hInst = GetModuleHandle(NULL);
	#else
		HINSTANCE hInst = GetModuleHandle("BansheeD3D9RenderAPI.dll");
	#endif

		RenderAPICore::startUp<D3D9RenderAPI>(hInst);
	}

	D3D9RenderAPIFactory::InitOnStart D3D9RenderAPIFactory::initOnStart;
}