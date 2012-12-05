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
#include "CmD3D9GpuProgram.h"
#include "CmMatrix4.h"
#include "CmException.h"
#include "CmD3D9Mappings.h"
#include "CmD3D9RenderSystem.h"
#include "CmAsyncOp.h"

namespace CamelotEngine {
    //-----------------------------------------------------------------------------
    D3D9GpuProgram::D3D9GpuProgram(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype, GpuProgramProfile profile) 
        : GpuProgram(source, entryPoint, language, gptype, profile), mpExternalMicrocode(NULL), mColumnMajorMatrices(false)
    {
    }

	//-----------------------------------------------------------------------------
	D3D9GpuProgram::~D3D9GpuProgram()
	{

	}
    
	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::setExternalMicrocode(const void* pMicrocode, size_t size)
	{
		LPD3DXBUFFER pBuffer=0;
		HRESULT hr=D3DXCreateBuffer(size, &pBuffer);
		if(pBuffer)
		{
			memcpy(pBuffer->GetBufferPointer(), pMicrocode, size);
			this->setExternalMicrocode(pBuffer);
			SAFE_RELEASE(pBuffer);
		}
	}
	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::setExternalMicrocode(ID3DXBuffer* pMicrocode)
	{ 
		SAFE_RELEASE(mpExternalMicrocode);
		mpExternalMicrocode = pMicrocode;
		if(mpExternalMicrocode)
		{
			mpExternalMicrocode->AddRef();	
		}
	}
    //-----------------------------------------------------------------------------
	LPD3DXBUFFER D3D9GpuProgram::getExternalMicrocode(void)
	{
		return mpExternalMicrocode;
	}

	//-----------------------------------------------------------------------------
    void D3D9GpuProgram::initialize_internal(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		for (UINT32 i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
		{
			IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

			initialize_internal(d3d9Device);
		}		       

		Resource::initialize_internal();
    }

	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::initialize_internal(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		if (mpExternalMicrocode)
		{
			loadFromMicrocode(d3d9Device, mpExternalMicrocode);
		}
		else
		{
			// Call polymorphic load
			loadFromSource(d3d9Device);
		}
	}
	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::unload(void)
	{
		SAFE_RELEASE(mpExternalMicrocode);
	}
	//-----------------------------------------------------------------------------
    void D3D9GpuProgram::loadFromSource(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		for (UINT32 i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
		{
			IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);

			loadFromSource(d3d9Device);
		}
    }

	//-----------------------------------------------------------------------------
	void D3D9GpuProgram::loadFromSource(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION


		// Populate compile flags
        DWORD compileFlags = 0;

		// Create the shader
		// Assemble source into microcode
		LPD3DXBUFFER microcode;
		LPD3DXBUFFER errors;
		HRESULT hr = D3DXAssembleShader(
			mSource.c_str(),
			static_cast<UINT>(mSource.length()),
			NULL,               // no #define support
			NULL,               // no #include support
			compileFlags,       // standard compile options
			&microcode,
			&errors);

		if (FAILED(hr))
		{
			String message = "Cannot assemble D3D9 shader. Errors:\n";
			message += static_cast<const char*>(errors->GetBufferPointer());

			errors->Release();
			CM_EXCEPT(RenderingAPIException, message);
		}
	
		loadFromMicrocode(d3d9Device, microcode);		

		SAFE_RELEASE(microcode);
		SAFE_RELEASE(errors);
	}
    //-----------------------------------------------------------------------
    void D3D9GpuProgram::createParameters_internal(AsyncOp& op)
    {
        // Call superclass
        GpuProgram::createParameters_internal(op);

		GpuProgramParametersSharedPtr params = op.getReturnValue<GpuProgramParametersSharedPtr>();

        // Need to transpose matrices if compiled with column-major matrices
        params->setTransposeMatrices(mColumnMajorMatrices);
    }	
	//-----------------------------------------------------------------------------
	D3D9GpuVertexProgram::D3D9GpuVertexProgram(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype, GpuProgramProfile profile) 
        : D3D9GpuProgram(source, entryPoint, language, gptype, profile)       
    {
        mType = GPT_VERTEX_PROGRAM;		
    }
	//-----------------------------------------------------------------------------
	D3D9GpuVertexProgram::~D3D9GpuVertexProgram()
	{	
	}
	//-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {		 
		DeviceToVertexShaderIterator it = mMapDeviceToVertexShader.find(d3d9Device);

		if (it != mMapDeviceToVertexShader.end())
			SAFE_RELEASE(it->second);

		if (isSupported())
		{
			// Create the shader
			IDirect3DVertexShader9* pVertexShader;
			HRESULT hr;
			
			hr = d3d9Device->CreateVertexShader( 
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				&pVertexShader);

			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Cannot create D3D9 vertex shader from microcode");
			}

			mMapDeviceToVertexShader[d3d9Device] = pVertexShader;
		}
		else
		{
			CM_EXCEPT(RenderingAPIException, "Specified D3D9 vertex shader is not supported!");

			// TODO LOG PORT - Log this error somewhere
			//LogManager::getSingleton().logMessage(
			//	"Unsupported D3D9 vertex shader '" + mName + "' was not loaded.");

			mMapDeviceToVertexShader[d3d9Device] = NULL;
		}
    }
	//-----------------------------------------------------------------------------
    void D3D9GpuVertexProgram::unload(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

        DeviceToVertexShaderIterator it = mMapDeviceToVertexShader.begin();

		while (it != mMapDeviceToVertexShader.end())
		{
			SAFE_RELEASE(it->second);
			++it;
		}
		mMapDeviceToVertexShader.clear();		
		D3D9GpuProgram::unload();
    }

	//-----------------------------------------------------------------------------
	void D3D9GpuVertexProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		
	}

	//-----------------------------------------------------------------------------
	void D3D9GpuVertexProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToVertexShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToVertexShader.find(d3d9Device);

		// Case shader found -> release it and erase from map.
		if (it != mMapDeviceToVertexShader.end())
		{
			SAFE_RELEASE(it->second);
			mMapDeviceToVertexShader.erase(it);
		}
	}

	//-----------------------------------------------------------------------------
	IDirect3DVertexShader9* D3D9GpuVertexProgram::getVertexShader( void )
	{
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		DeviceToVertexShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToVertexShader.find(d3d9Device);
		
		// Shader was not found -> load it.
		if (it == mMapDeviceToVertexShader.end())		
		{
			initialize_internal(d3d9Device);		
			it = mMapDeviceToVertexShader.find(d3d9Device);
		}
	
		return it->second;
	}

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
    D3D9GpuFragmentProgram::D3D9GpuFragmentProgram(const String& source, const String& entryPoint, const String& language, GpuProgramType gptype, GpuProgramProfile profile) 
		: D3D9GpuProgram(source, entryPoint, language, gptype, profile)       
    {
        mType = GPT_FRAGMENT_PROGRAM;
    }
	//-----------------------------------------------------------------------------
	D3D9GpuFragmentProgram::~D3D9GpuFragmentProgram()
	{
	}
	//-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {
		DeviceToPixelShaderIterator it = mMapDeviceToPixelShader.find(d3d9Device);

		if (it != mMapDeviceToPixelShader.end())
			SAFE_RELEASE(it->second);

		if (isSupported())
		{
			// Create the shader
			IDirect3DPixelShader9* pPixelShader;
			HRESULT hr;

			hr = d3d9Device->CreatePixelShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				&pPixelShader);

			if (FAILED(hr))
			{
				CM_EXCEPT(RenderingAPIException, "Cannot create D3D9 pixel shader from microcode.");
			}

			mMapDeviceToPixelShader[d3d9Device] = pPixelShader;
		}
		else
		{
			// TODO LOG PORT - Log this somewhere
			//LogManager::getSingleton().logMessage(
			//	"Unsupported D3D9 pixel shader was not loaded.");

			mMapDeviceToPixelShader[d3d9Device] = NULL;
		}
    }
	//-----------------------------------------------------------------------------
    void D3D9GpuFragmentProgram::unload(void)
    {
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToPixelShaderIterator it = mMapDeviceToPixelShader.begin();

		while (it != mMapDeviceToPixelShader.end())
		{
			SAFE_RELEASE(it->second);
			++it;
		}
		mMapDeviceToPixelShader.clear();	
		D3D9GpuProgram::unload();
    }
	//-----------------------------------------------------------------------------
	void D3D9GpuFragmentProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		
	}

	//-----------------------------------------------------------------------------
	void D3D9GpuFragmentProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		D3D9_DEVICE_ACCESS_CRITICAL_SECTION

		DeviceToPixelShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToPixelShader.find(d3d9Device);

		// Case shader found -> release it and erase from map.
		if (it != mMapDeviceToPixelShader.end())
		{
			SAFE_RELEASE(it->second);
			mMapDeviceToPixelShader.erase(it);
		}
	}

	//-----------------------------------------------------------------------------
	IDirect3DPixelShader9* D3D9GpuFragmentProgram::getPixelShader( void )
	{
		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		DeviceToPixelShaderIterator it;

		// Find the shader of this device.
		it = mMapDeviceToPixelShader.find(d3d9Device);

		// Shader was not found -> load it.
		if (it == mMapDeviceToPixelShader.end())		
		{
			initialize_internal(d3d9Device);			
			it = mMapDeviceToPixelShader.find(d3d9Device);
		}

		return it->second;
	}
	//-----------------------------------------------------------------------------

}

