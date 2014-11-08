#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9Resource.h"
#include "BsRenderTexture.h"

namespace BansheeEngine
{
	class D3D9RenderTexture;

	/**
	 * @brief	DirectX 9 implementation of a render texture.
	 *
	 * @note	Core thread only.
	 */
	class BS_D3D9_EXPORT D3D9RenderTextureCore : public RenderTextureCore, public D3D9Resource
	{
	public:
		D3D9RenderTextureCore(D3D9RenderTexture* parent, RenderTextureProperties* properties, const RENDER_SURFACE_DESC& colorSurfaceDesc,
			const RENDER_SURFACE_DESC& depthStencilSurfaceDesc);

		virtual ~D3D9RenderTextureCore();

		/**
		 * @copydoc	RenderTextureCore::getCustomAttribute
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
		friend class D3D9RenderTexture;

		/**
		 * @copydoc	CoreObjectCore::initialize
		 */
		virtual void initialize();

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

	/**
	* @brief	DirectX 9 implementation of a render texture.
	*
	* @note		Sim thread only.
	*/
	class BS_D3D9_EXPORT D3D9RenderTexture : public RenderTexture
	{
	public:
		virtual ~D3D9RenderTexture() { }

	protected:
		friend class D3D9TextureManager;

		D3D9RenderTexture() { }

		/**
		 * @copydoc	RenderTexture::createProperties
		 */
		virtual RenderTargetProperties* createProperties() const;

		/**
		 * @copydoc	RenderTexture::createCore
		 */
		virtual SPtr<CoreObjectCore> createCore() const;
	};
}