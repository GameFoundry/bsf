#include "BsD3D9GpuProgram.h"
#include "BsMatrix4.h"
#include "BsException.h"
#include "BsD3D9Mappings.h"
#include "BsD3D9RenderSystem.h"
#include "BsD3D9ResourceManager.h"
#include "BsGpuParams.h"
#include "BsAsyncOp.h"
#include "BsGpuProgramManager.h"
#include "BsD3D9HLSLParamParser.h"
#include "BsD3D9GpuProgramRTTI.h"
#include "BsRenderStats.h"

namespace BansheeEngine 
{
    D3D9GpuProgram::D3D9GpuProgram(const String& source, const String& entryPoint, 
		GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes)
		: GpuProgram(source, entryPoint, gptype, profile, includes, false),
		mMicrocode(nullptr), mColumnMajorMatrices(false), mOptimisationLevel(OPT_DEFAULT)
    { }

	D3D9GpuProgram::~D3D9GpuProgram()
	{ }

	void D3D9GpuProgram::initialize_internal()
	{
		if (!isSupported())
		{
			mIsCompiled = false;
			mCompileError = "Specified program is not supported by the current render system.";

			GpuProgram::initialize_internal();
			return;
		}

		// Populate preprocessor defines
		String stringBuffer;

		Vector<D3DXMACRO> defines;
		const D3DXMACRO* pDefines = 0;
		if (!mPreprocessorDefines.empty())
		{
			stringBuffer = mPreprocessorDefines;

			// Split preprocessor defines and build up macro array
			D3DXMACRO macro;
			String::size_type pos = 0;
			while (pos != String::npos)
			{
				macro.Name = &stringBuffer[pos];
				macro.Definition = 0;

				String::size_type start_pos = pos;

				// Find delims
				pos = stringBuffer.find_first_of(";,=", pos);

				if (start_pos == pos)
				{
					if (pos == stringBuffer.length())
					{
						break;
					}
					pos++;
					continue;
				}

				if (pos != String::npos)
				{
					// Check definition part
					if (stringBuffer[pos] == '=')
					{
						// Setup null character for macro name
						stringBuffer[pos++] = '\0';
						macro.Definition = &stringBuffer[pos];
						pos = stringBuffer.find_first_of(";,", pos);
					}
					else
					{
						// No definition part, define as "1"
						macro.Definition = "1";
					}

					if (pos != String::npos)
					{
						// Setup null character for macro name or definition
						stringBuffer[pos++] = '\0';
					}
				}
				else
				{
					macro.Definition = "1";
				}
				if (strlen(macro.Name) > 0)
				{
					defines.push_back(macro);
				}
				else
				{
					break;
				}
			}

			// Add NULL terminator
			macro.Name = 0;
			macro.Definition = 0;
			defines.push_back(macro);

			pDefines = &defines[0];
		}

		// Populate compile flags
		DWORD compileFlags = 0;
		if (mColumnMajorMatrices)
			compileFlags |= D3DXSHADER_PACKMATRIX_COLUMNMAJOR;
		else
			compileFlags |= D3DXSHADER_PACKMATRIX_ROWMAJOR;

#if BS_DEBUG_MODE
		compileFlags |= D3DXSHADER_DEBUG;
#endif
		switch (mOptimisationLevel)
		{
		case OPT_DEFAULT:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL1;
			break;
		case OPT_NONE:
			compileFlags |= D3DXSHADER_SKIPOPTIMIZATION;
			break;
		case OPT_0:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL0;
			break;
		case OPT_1:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL1;
			break;
		case OPT_2:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL2;
			break;
		case OPT_3:
			compileFlags |= D3DXSHADER_OPTIMIZATION_LEVEL3;
			break;
		}

		LPD3DXBUFFER errors = 0;

		// include handler
		LPD3DXCONSTANTTABLE constTable;

		String hlslProfile = D3D9RenderSystem::instance().getCapabilities()->gpuProgProfileToRSSpecificProfile(mProfile);

		String parsedSource = D3D9EmulatedParamBlockParser::parse(mSource, mBlocks);

		// Compile & assemble into microcode
		HRESULT hr = D3DXCompileShader(
			parsedSource.c_str(),
			static_cast<UINT>(parsedSource.length()),
			pDefines,
			nullptr,
			mEntryPoint.c_str(),
			hlslProfile.c_str(),
			compileFlags,
			&mMicrocode,
			&errors,
			&constTable);

		if (FAILED(hr))
		{
			String message = "Cannot compile D3D9 high-level shader ";

			if (errors)
			{
				message += String(" Errors:\n") + static_cast<const char*>(errors->GetBufferPointer());
				errors->Release();
			}

			mIsCompiled = false;
			mCompileError = message;

			SAFE_RELEASE(mMicrocode);
			mMicrocode = nullptr;
		}
		else
		{
			for (UINT32 i = 0; i < D3D9RenderSystem::getResourceCreationDeviceCount(); ++i)
			{
				IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getResourceCreationDevice(i);
				loadFromMicrocode(d3d9Device, mMicrocode);
			}

			D3D9HLSLParamParser paramParser(constTable, mBlocks);
			mParametersDesc = paramParser.buildParameterDescriptions();

			mIsCompiled = true;
			mCompileError = "";

			SAFE_RELEASE(constTable);
		}

		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_GpuProgram);
		GpuProgram::initialize_internal();
	}

	void D3D9GpuProgram::destroy_internal()
	{
		SAFE_RELEASE(mMicrocode);

		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_GpuProgram);
		GpuProgram::destroy_internal();
	}

	GpuParamsPtr D3D9GpuProgram::createParameters()
	{
		GpuParamsPtr params = bs_shared_ptr<GpuParams, PoolAlloc>(std::ref(mParametersDesc), mColumnMajorMatrices);

		return params;
	}

	const String& D3D9GpuProgram::getLanguage() const
	{
		static const String language = "hlsl";

		return language;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* D3D9GpuProgram::getRTTIStatic()
	{
		return D3D9GpuProgramRTTI::instance();
	}

	RTTITypeBase* D3D9GpuProgram::getRTTI() const
	{
		return D3D9GpuProgram::getRTTIStatic();
	}

	D3D9GpuVertexProgram::D3D9GpuVertexProgram(const String& source, const String& entryPoint, 
		GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes)
		: D3D9GpuProgram(source, entryPoint, GPT_VERTEX_PROGRAM, profile, includes)
    {

    }

	D3D9GpuVertexProgram::~D3D9GpuVertexProgram()
	{	
	}

	void D3D9GpuVertexProgram::destroy_internal(void)
	{
		auto it = mMapDeviceToVertexShader.begin();

		while (it != mMapDeviceToVertexShader.end())
		{
			SAFE_RELEASE(it->second);
			++it;
		}
		mMapDeviceToVertexShader.clear();	

		D3D9GpuProgram::destroy_internal();
	}

    void D3D9GpuVertexProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {		 
		auto it = mMapDeviceToVertexShader.find(d3d9Device);

		if (it != mMapDeviceToVertexShader.end())
			SAFE_RELEASE(it->second);

		// Create the shader
		IDirect3DVertexShader9* pVertexShader;
		HRESULT hr;
			
		hr = d3d9Device->CreateVertexShader( 
			static_cast<DWORD*>(microcode->GetBufferPointer()), 
			&pVertexShader);

		if (FAILED(hr))
		{
			BS_EXCEPT(RenderingAPIException, "Cannot create D3D9 vertex shader from microcode");
		}

		mMapDeviceToVertexShader[d3d9Device] = pVertexShader;
    }

	void D3D9GpuVertexProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		
	}

	void D3D9GpuVertexProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		auto it = mMapDeviceToVertexShader.find(d3d9Device);

		// Case shader found -> release it and erase from map.
		if (it != mMapDeviceToVertexShader.end())
		{
			SAFE_RELEASE(it->second);
			mMapDeviceToVertexShader.erase(it);
		}
	}

	IDirect3DVertexShader9* D3D9GpuVertexProgram::getVertexShader()
	{
		if (!isCompiled())
			return nullptr;

		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		auto it = mMapDeviceToVertexShader.find(d3d9Device);

		// Shader was not found -> load it.
		if (it == mMapDeviceToVertexShader.end())		
		{
			loadFromMicrocode(d3d9Device, mMicrocode);
			it = mMapDeviceToVertexShader.find(d3d9Device);
		}
	
		return it->second;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* D3D9GpuVertexProgram::getRTTIStatic()
	{
		return D3D9GpuVertexProgramRTTI::instance();
	}

	RTTITypeBase* D3D9GpuVertexProgram::getRTTI() const
	{
		return D3D9GpuVertexProgram::getRTTIStatic();
	}

    D3D9GpuFragmentProgram::D3D9GpuFragmentProgram(const String& source, const String& entryPoint, 
		GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes)
		: D3D9GpuProgram(source, entryPoint, GPT_FRAGMENT_PROGRAM, profile, includes)
    {

    }

	D3D9GpuFragmentProgram::~D3D9GpuFragmentProgram()
	{
	}

	void D3D9GpuFragmentProgram::destroy_internal()
	{
		auto it = mMapDeviceToPixelShader.begin();

		while (it != mMapDeviceToPixelShader.end())
		{
			SAFE_RELEASE(it->second);
			++it;
		}
		mMapDeviceToPixelShader.clear();	

		D3D9GpuProgram::destroy_internal();
	}

    void D3D9GpuFragmentProgram::loadFromMicrocode(IDirect3DDevice9* d3d9Device, ID3DXBuffer* microcode)
    {
		auto it = mMapDeviceToPixelShader.find(d3d9Device);

		if (it != mMapDeviceToPixelShader.end())
			SAFE_RELEASE(it->second);

		// Create the shader
		IDirect3DPixelShader9* pPixelShader;
		HRESULT hr;

		hr = d3d9Device->CreatePixelShader(static_cast<DWORD*>(microcode->GetBufferPointer()), &pPixelShader);

		if (FAILED(hr))
			BS_EXCEPT(RenderingAPIException, "Cannot create D3D9 pixel shader from microcode.");

		mMapDeviceToPixelShader[d3d9Device] = pPixelShader;
    }

	void D3D9GpuFragmentProgram::notifyOnDeviceCreate(IDirect3DDevice9* d3d9Device)
	{
		
	}

	void D3D9GpuFragmentProgram::notifyOnDeviceDestroy(IDirect3DDevice9* d3d9Device)
	{
		auto it = mMapDeviceToPixelShader.find(d3d9Device);

		// Case shader found -> release it and erase from map.
		if (it != mMapDeviceToPixelShader.end())
		{
			SAFE_RELEASE(it->second);
			mMapDeviceToPixelShader.erase(it);
		}
	}

	IDirect3DPixelShader9* D3D9GpuFragmentProgram::getPixelShader()
	{
		if (!isCompiled())
			return nullptr;

		IDirect3DDevice9* d3d9Device = D3D9RenderSystem::getActiveD3D9Device();
		auto it = mMapDeviceToPixelShader.find(d3d9Device);

		// Shader was not found -> load it.
		if (it == mMapDeviceToPixelShader.end())		
		{
			loadFromMicrocode(d3d9Device, mMicrocode);
			it = mMapDeviceToPixelShader.find(d3d9Device);
		}

		return it->second;
	}

	/************************************************************************/
	/* 								SERIALIZATION                      		*/
	/************************************************************************/
	RTTITypeBase* D3D9GpuFragmentProgram::getRTTIStatic()
	{
		return D3D9GpuFragmentProgramRTTI::instance();
	}

	RTTITypeBase* D3D9GpuFragmentProgram::getRTTI() const
	{
		return D3D9GpuFragmentProgram::getRTTIStatic();
	}
}

