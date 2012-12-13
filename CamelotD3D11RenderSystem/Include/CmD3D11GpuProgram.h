#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuProgram.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11GpuProgram : public GpuProgram
	{
	public:
		D3D11GpuProgram(GpuProgramType type, GpuProgramProfile profile);

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
		D3D11GpuVertexProgram(GpuProgramProfile profile);
		~D3D11GpuVertexProgram();

		ID3D11VertexShader* getVertexShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	class CM_D3D11_EXPORT D3D11GpuFragmentProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11PixelShader* mPixelShader;
	public:
		D3D11GpuFragmentProgram(GpuProgramProfile profile);
		~D3D11GpuFragmentProgram();

		ID3D11PixelShader* getPixelShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	class D3D11GpuDomainProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11DomainShader* mDomainShader;
	public:
		D3D11GpuDomainProgram(GpuProgramProfile profile);
		~D3D11GpuDomainProgram();

		ID3D11DomainShader* getDomainShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	class D3D11GpuHullProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11HullShader* mHullShader;
	public:
		D3D11GpuHullProgram(GpuProgramProfile profile);
		~D3D11GpuHullProgram();

		ID3D11HullShader* getHullShader() const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	class D3D11GpuGeometryProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11GeometryShader* mGeometryShader;
	public:
		D3D11GpuGeometryProgram(GpuProgramProfile profile);
		~D3D11GpuGeometryProgram();

		ID3D11GeometryShader* getGeometryShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};
}