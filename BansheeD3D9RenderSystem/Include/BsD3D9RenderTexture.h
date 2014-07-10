//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Resource.h"
#include "BsRenderTexture.h"

namespace BansheeEngine
{
	/**
	 * @brief	DirectX 9 implementation of a render texture.
	 */
	class BS_D3D9_EXPORT D3D9RenderTexture : public RenderTexture, public D3D9Resource
	{
	public:
		virtual ~D3D9RenderTexture();

		/**
		 * @copydoc	RenderTexture::requiresTextureFlipping
		 */
		virtual bool requiresTextureFlipping() const { return false; }

		/**
		 * @copydoc	RenderTexture::getCustomAttribute
		 */
		virtual void getCustomAttribute(const String& name, void* pData) const;

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceLost
		 */
		virtual void notifyOnDeviceLost(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceReset
		 */
		virtual void notifyOnDeviceReset(IDirect3DDevice9* d3d9Device);

	protected:
		friend class D3D9TextureManager;

		D3D9RenderTexture();

		/**
		 * @copydoc RenderTexture::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @brief	Initializes the internal color and depth surfaces.
		 */
		void initializeSurfaces();

		/**
		 * @brief	Releases the internal color and depth surfaces.
		 */
		void releaseSurfaces();

	protected:
		IDirect3DSurface9* mDX9ColorSurface;
		IDirect3DSurface9* mDX9DepthStencilSurface;
		bool mIsBindableToShader;
	};
}