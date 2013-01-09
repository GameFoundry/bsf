#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuProgram.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11GpuProgram : public GpuProgram
	{
	public:
		D3D11GpuProgram(GpuProgramType type);

	protected:
		void loadImpl(void);

		/**
		 * @brief	Loads shader from source. Not used as DX11 doesn't support
		 * 			assembly shaders.
		 */
		void loadFromSource(void);

		/**
		 * @brief	Loads shader from microcode.
		 */
		virtual void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode) = 0;
	};

	class CM_D3D11_EXPORT D3D11GpuVertexProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11VertexShader* mVertexShader;
	public:
		D3D11GpuVertexProgram();
		~D3D11GpuVertexProgram();

		ID3D11VertexShader* getVertexShader(void) const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		void unloadImpl(void);
	};

	class CM_D3D11_EXPORT D3D11GpuFragmentProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11PixelShader* mPixelShader;
	public:
		D3D11GpuFragmentProgram();
		~D3D11GpuFragmentProgram();

		ID3D11PixelShader* getPixelShader(void) const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		void unloadImpl(void);
	};

	class D3D11GpuDomainProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11DomainShader* mDomainShader;
	public:
		D3D11GpuDomainProgram();
		~D3D11GpuDomainProgram();

		ID3D11DomainShader* getDomainShader(void) const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		void unloadImpl(void);
	};

	class D3D11GpuHullProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11HullShader* mHullShader;
	public:
		D3D11GpuHullProgram();
		~D3D11GpuHullProgram();

		ID3D11HullShader* getHullShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		void unloadImpl(void);
	};

	class D3D11GpuGeometryProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11GeometryShader* mGeometryShader;
	public:
		D3D11GpuGeometryProgram();
		~D3D11GpuGeometryProgram();

		ID3D11GeometryShader* getGeometryShader(void) const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		void unloadImpl(void);
	};

	class D3D11GpuComputeProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11ComputeShader* mComputeShader;
	public:
		D3D11GpuComputeProgram();
		~D3D11GpuComputeProgram();

		ID3D11ComputeShader* getComputeShader(void) const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		void unloadImpl(void);
	};
}