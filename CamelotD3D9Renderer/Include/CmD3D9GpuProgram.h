/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __D3D9GpuProgram_H_
#define __D3D9GpuProgram_H_

// Precompiler options
#include "CmD3D9Prerequisites.h"
#include "CmGpuProgram.h"
#include "CmD3D9Resource.h"

namespace BansheeEngine {

    /** Direct3D implementation of a few things common to low-level vertex & fragment programs. */
    class CM_D3D9_EXPORT D3D9GpuProgram : public GpuProgram, public D3D9Resource
    {   
    public:
        ~D3D9GpuProgram();

        /** Sets whether matrix packing in column-major order. */ 
        void setColumnMajorMatrices(bool columnMajor) { mColumnMajorMatrices = columnMajor; }
        /** Gets whether matrix packed in column-major order. */
        bool getColumnMajorMatrices(void) const { return mColumnMajorMatrices; }

		/** Tells the program to load from some externally created microcode instead of a file or source. 
		*/
		void setExternalMicrocode(const void* pMicrocode, UINT32 size);
        /** Tells the program to load from some externally created microcode instead of a file or source. 
        @remarks
            add ref count to pMicrocode when setting
        */ 
        void setExternalMicrocode(ID3DXBuffer* pMicrocode);
        /** Gets the external microcode buffer, if any. */
        LPD3DXBUFFER getExternalMicrocode(void);

		bool isSupported() const;
    protected:
		friend class D3D9GpuProgramManager;

		D3D9GpuProgram(const String& source, const String& entryPoint, 
			GpuProgramType gptype, GpuProgramProfile profile);

		void createInternalResources(IDirect3DDevice9* d3d9Device);

		/**
		 * @copydoc GpuProgram::initialize_internal().
		 */
		void initialize_internal();

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();
      
		/** Loads this program from microcode, must be overridden by subclasses. */
        virtual void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode) = 0;

        /** Creates a new parameters object compatible with this program definition. 
        @remarks
            It is recommended that you use this method of creating parameters objects
            rather than going direct to GpuProgramManager, because this method will
            populate any implementation-specific extras (like named parameters) where
            they are appropriate.
        */
        virtual GpuParamsPtr createParameters();
	protected:    
		bool mColumnMajorMatrices;
		ID3DXBuffer* mpExternalMicrocode;

    };

    /** Direct3D implementation of low-level vertex programs. */
    class CM_D3D9_EXPORT D3D9GpuVertexProgram : public D3D9GpuProgram
    {  
    public:
		~D3D9GpuVertexProgram();
        
		/// Gets the vertex shader
        IDirect3DVertexShader9* getVertexShader(void);

		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

    protected:
		friend class D3D9GpuProgramManager;

		D3D9GpuVertexProgram(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile);

		/**
		 * @copydoc D3D9GpuProgram::destroy_internal.
		 */
		void destroy_internal();

        void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode);

	protected:
		typedef Map<IDirect3DDevice9*, IDirect3DVertexShader9*>::type   DeviceToVertexShaderMap;
		typedef DeviceToVertexShaderMap::iterator						DeviceToVertexShaderIterator;
	
		DeviceToVertexShaderMap		mMapDeviceToVertexShader;	
    };

    /** Direct3D implementation of low-level fragment programs. */
    class CM_D3D9_EXPORT D3D9GpuFragmentProgram : public D3D9GpuProgram
    {  
    public:
		~D3D9GpuFragmentProgram();

        /// Gets the pixel shader
        IDirect3DPixelShader9* getPixelShader(void);

		// Called immediately after the Direct3D device has been created.
		virtual void notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device);

		// Called before the Direct3D device is going to be destroyed.
		virtual void notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device);

    protected:
		friend class D3D9GpuProgramManager;

		D3D9GpuFragmentProgram(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile);

		/**
		 * @copydoc D3D9GpuProgram::destroy_internal.
		 */
		void destroy_internal();

        void loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode);

	protected:
		typedef Map<IDirect3DDevice9*, IDirect3DPixelShader9*>::type	DeviceToPixelShaderMap;
		typedef DeviceToPixelShaderMap::iterator						DeviceToPixelShaderIterator;

		DeviceToPixelShaderMap		mMapDeviceToPixelShader;			
    };

	typedef std::shared_ptr<D3D9GpuProgram> D3D9GpuProgramPtr;
}


#endif
