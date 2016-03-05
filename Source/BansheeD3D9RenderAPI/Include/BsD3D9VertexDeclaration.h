//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsVertexDeclaration.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{ 
	/**
	 * @brief	DirectX 9 implementation of a vertex declaration.
	 */
    class BS_D3D9_EXPORT D3D9VertexDeclarationCore : public VertexDeclarationCore, public D3D9Resource
    {
    public:
		~D3D9VertexDeclarationCore();
        
		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device) override;

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device) override;

		/**
		 * @brief	Creates a DirectX 9 vertex declaration object.
		 */
        IDirect3DVertexDeclaration9* getD3DVertexDeclaration();

	protected:
		friend class D3D9HardwareBufferCoreManager;

		D3D9VertexDeclarationCore(const List<VertexElement>& elements);

		/**
		 * @brief	Releases the internal DirectX 9 vertex declaration object.
		 */
		void releaseDeclaration();

	protected:        
		Map<IDirect3DDevice9*, IDirect3DVertexDeclaration9*> mMapDeviceToDeclaration;
    };
}