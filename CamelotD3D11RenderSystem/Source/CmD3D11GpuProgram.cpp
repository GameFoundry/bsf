#include "CmD3D11GpuProgram.h"
#include "CmD3D11Device.h"
#include "CmException.h"
#include "CmDebug.h"

namespace CamelotEngine
{
	D3D11GpuProgram::D3D11GpuProgram(GpuProgramType type, const String& profile) 
		: GpuProgram("", "", profile, type, GPP_NONE)
	{

	}

	D3D11GpuVertexProgram::D3D11GpuVertexProgram(const String& profile) 
		: D3D11GpuProgram(GPT_VERTEX_PROGRAM, profile)
		, mVertexShader(nullptr)
	{ }

	D3D11GpuVertexProgram::~D3D11GpuVertexProgram()
	{ }

	void D3D11GpuVertexProgram::loadFromMicrocode(D3D11Device& device, ID3D10Blob*  microcode)
	{
		if (isSupported())
		{
			// Create the shader
			HRESULT hr = device.getD3D11Device()->CreateVertexShader( 
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				microcode->GetBufferSize(),
				device.getClassLinkage(),
				&mVertexShader);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, 
					"Cannot create D3D11 vertex shader from microcode\nError Description:" + errorDescription);

			}
		}
		else
		{
			LOGWRN("Unsupported D3D11 vertex shader was not loaded.");
		}
	}

	void D3D11GpuVertexProgram::destroy_internal()
	{
		SAFE_RELEASE(mVertexShader);

		D3D11GpuProgram::destroy_internal();
	}

	ID3D11VertexShader * D3D11GpuVertexProgram::getVertexShader( void ) const
	{
		return mVertexShader;
	}

	D3D11GpuFragmentProgram::D3D11GpuFragmentProgram(const String& profile) 
		: D3D11GpuProgram(GPT_FRAGMENT_PROGRAM, profile)
		, mPixelShader(nullptr)
	{ }

	D3D11GpuFragmentProgram::~D3D11GpuFragmentProgram()
	{ }

	void D3D11GpuFragmentProgram::loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode)
	{
		if (isSupported())
		{
			// Create the shader
			HRESULT hr = device.getD3D11Device()->CreatePixelShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				microcode->GetBufferSize(),
				device.getClassLinkage(),
				&mPixelShader);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, 
					"Cannot create D3D11 pixel shader from microcode.\nError Description:" + errorDescription);
			}
		}
		else
		{
			LOGWRN("Unsupported D3D11 pixel shader was not loaded.");
		}
	}

	void D3D11GpuFragmentProgram::destroy_internal()
	{
		SAFE_RELEASE(mPixelShader);

		D3D11GpuProgram::destroy_internal();
	}

	ID3D11PixelShader * D3D11GpuFragmentProgram::getPixelShader( void ) const
	{
		return mPixelShader;
	}

	D3D11GpuGeometryProgram::D3D11GpuGeometryProgram(const String& profile) 
		: D3D11GpuProgram(GPT_GEOMETRY_PROGRAM, profile)
		, mGeometryShader(nullptr)
	{ }

	D3D11GpuGeometryProgram::~D3D11GpuGeometryProgram()
	{ }

	void D3D11GpuGeometryProgram::loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode)
	{
		if (isSupported())
		{
			// Create the shader
			HRESULT hr = device.getD3D11Device()->CreateGeometryShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				microcode->GetBufferSize(),
				device.getClassLinkage(),
				&mGeometryShader);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, 
					"Cannot create D3D11 geometry shader from microcode.\nError Description:" + errorDescription);
			}
		}
		else
		{
			LOGWRN("Unsupported D3D11 geometry shader was not loaded.");
		}
	}

	void D3D11GpuGeometryProgram::destroy_internal()
	{
		SAFE_RELEASE(mGeometryShader);

		D3D11GpuProgram::destroy_internal();
	}

	ID3D11GeometryShader * D3D11GpuGeometryProgram::getGeometryShader(void) const
	{
		return mGeometryShader;
	}

	D3D11GpuDomainProgram::D3D11GpuDomainProgram(const String& profile) 
		: D3D11GpuProgram(GPT_DOMAIN_PROGRAM, profile)
		, mDomainShader(nullptr)
	{ }

	D3D11GpuDomainProgram::~D3D11GpuDomainProgram()
	{ }

	void D3D11GpuDomainProgram::loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode)
	{
		if (isSupported())
		{
			// Create the shader
			HRESULT hr = device.getD3D11Device()->CreateDomainShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				microcode->GetBufferSize(),
				device.getClassLinkage(),
				&mDomainShader);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, 
					"Cannot create D3D11 domain shader from microcode.\nError Description:" + errorDescription);
			}
		}
		else
		{
			LOGWRN("Unsupported D3D11 domain shader was not loaded.");
		}
	}

	void D3D11GpuDomainProgram::destroy_internal()
	{
		SAFE_RELEASE(mDomainShader);

		D3D11GpuProgram::destroy_internal();
	}

	ID3D11DomainShader * D3D11GpuDomainProgram::getDomainShader(void) const
	{
		return mDomainShader;
	}

	D3D11GpuHullProgram::D3D11GpuHullProgram(const String& profile) 
		: D3D11GpuProgram(GPT_HULL_PROGRAM, profile)
		, mHullShader(nullptr)
	{ }

	D3D11GpuHullProgram::~D3D11GpuHullProgram()
	{ }

	void D3D11GpuHullProgram::loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode)
	{
		if (isSupported())
		{
			// Create the shader
			HRESULT hr = device.getD3D11Device()->CreateHullShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				microcode->GetBufferSize(),
				device.getClassLinkage(),
				&mHullShader);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, 
					"Cannot create D3D11 hull shader from microcode.\nError Description:" + errorDescription);
			}
		}
		else
		{
			LOGWRN("Unsupported D3D11 hull shader was not loaded.");
		}
	}

	void D3D11GpuHullProgram::destroy_internal()
	{
		SAFE_RELEASE(mHullShader);

		D3D11GpuProgram::destroy_internal();
	}

	ID3D11HullShader* D3D11GpuHullProgram::getHullShader(void) const
	{
		return mHullShader;
	}

	D3D11GpuComputeProgram::D3D11GpuComputeProgram(const String& profile) 
		: D3D11GpuProgram(GPT_COMPUTE_PROGRAM, profile)
		, mComputeShader(nullptr)
	{ }

	D3D11GpuComputeProgram::~D3D11GpuComputeProgram()
	{ }

	void D3D11GpuComputeProgram::loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode)
	{
		if (isSupported())
		{
			// Create the shader
			HRESULT hr = device.getD3D11Device()->CreateComputeShader(
				static_cast<DWORD*>(microcode->GetBufferPointer()), 
				microcode->GetBufferSize(),
				device.getClassLinkage(),
				&mComputeShader);

			if (FAILED(hr) || device.hasError())
			{
				String errorDescription = device.getErrorDescription();
				CM_EXCEPT(RenderingAPIException, 
					"Cannot create D3D11 compute shader from microcode.\nError Description:" + errorDescription);
			}
		}
		else
		{
			LOGWRN("Unsupported D3D11 compute shader was not loaded.");
		}
	}

	void D3D11GpuComputeProgram::destroy_internal()
	{
		SAFE_RELEASE(mComputeShader);

		D3D11GpuProgram::destroy_internal();
	}

	ID3D11ComputeShader* D3D11GpuComputeProgram::getComputeShader(void) const
	{
		return mComputeShader;
	}
}