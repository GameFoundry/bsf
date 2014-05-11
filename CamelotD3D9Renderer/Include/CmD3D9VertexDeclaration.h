#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmVertexDeclaration.h"
#include "CmD3D9Resource.h"

namespace BansheeEngine 
{ 
    /** Specialisation of VertexDeclaration for D3D9 */
    class CM_D3D9_EXPORT D3D9VertexDeclaration : public VertexDeclaration, public D3D9Resource
    {
    public:
        ~D3D9VertexDeclaration();
        
		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

        /** Gets the D3D9-specific vertex declaration. */
        IDirect3DVertexDeclaration9* getD3DVertexDeclaration();

	protected:
		friend class D3D9HardwareBufferManager;

		D3D9VertexDeclaration(const VertexDeclaration::VertexElementList& elements);

		void releaseDeclaration();

		/**
		 * @copydoc VertexDeclaration::destroy_internal().
		 */
		void destroy_internal();

	protected:        
		typedef Map<IDirect3DDevice9*, IDirect3DVertexDeclaration9*>	DeviceToDeclarationMap;
		typedef DeviceToDeclarationMap::iterator							DeviceToDeclarationIterator;

		DeviceToDeclarationMap		mMapDeviceToDeclaration;
    };
}