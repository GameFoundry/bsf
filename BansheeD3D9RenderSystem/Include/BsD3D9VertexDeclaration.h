//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsVertexDeclaration.h"
#include "BsD3D9Resource.h"

namespace BansheeEngine 
{ 
	/**
	 * @brief	DirectX 9 implementation of a vertex declaration.
	 */
    class BS_D3D9_EXPORT D3D9VertexDeclaration : public VertexDeclaration, public D3D9Resource
    {
    public:
        ~D3D9VertexDeclaration();
        
		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceCreate
		 */
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc	D3D9Resource::notifyOnDeviceDestroy
		 */
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

		/**
		 * @brief	Creates a DirectX 9 vertex declaration object.
		 */
        IDirect3DVertexDeclaration9* getD3DVertexDeclaration();

	protected:
		friend class D3D9HardwareBufferManager;

		D3D9VertexDeclaration(const VertexDeclaration::VertexElementList& elements);

		/**
		 * @brief	Releases the internal DirectX 9 vertex declaration object.
		 */
		void releaseDeclaration();

		/**
		 * @copydoc	VertexDeclaration::destroy_internal
		 */
		void destroy_internal();

	protected:        
		Map<IDirect3DDevice9*, IDirect3DVertexDeclaration9*> mMapDeviceToDeclaration;
    };
}