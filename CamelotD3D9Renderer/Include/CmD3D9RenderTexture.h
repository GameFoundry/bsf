#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmD3D9Resource.h"
#include "CmRenderTexture.h"

namespace BansheeEngine
{
	/// RenderTexture implementation for D3D9
	class CM_D3D9_EXPORT D3D9RenderTexture : public RenderTexture, public D3D9Resource
	{
	public:
		virtual ~D3D9RenderTexture();

		bool requiresTextureFlipping() const { return false; }
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