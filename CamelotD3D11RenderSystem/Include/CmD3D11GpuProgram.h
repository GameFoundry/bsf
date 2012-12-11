#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuProgram.h"

namespace CamelotEngine
{
	/** Direct3D implementation of a few things common to low-level vertex & fragment programs. */
	class D3D11GpuProgram : public GpuProgram
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

	/** Direct3D implementation of low-level vertex programs. */
	class D3D11GpuVertexProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11VertexShader* mVertexShader;
	public:
		D3D11GpuVertexProgram(GpuProgramProfile profile);
		~D3D11GpuVertexProgram();

		/// Gets the vertex shader
		ID3D11VertexShader* getVertexShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	/** Direct3D implementation of low-level fragment programs. */
	class D3D11GpuFragmentProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11PixelShader* mPixelShader;
	public:
		D3D11GpuFragmentProgram(GpuProgramProfile profile);
		~D3D11GpuFragmentProgram();

		/// Gets the pixel shader
		ID3D11PixelShader* getPixelShader(void) const;
	protected:
		/** @copydoc Resource::unloadImpl */
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	/** Direct3D implementation of low-level vertex programs. */
	class D3D11GpuDomainProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11DomainShader* mDomainShader;
	public:
		D3D11GpuDomainProgram(GpuProgramProfile profile);
		~D3D11GpuDomainProgram();

		/// Gets the vertex shader
		ID3D11DomainShader* getDomainShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	/** Direct3D implementation of low-level vertex programs. */
	class D3D11GpuHullProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11HullShader* mHullShader;
	public:
		D3D11GpuHullProgram(GpuProgramProfile profile);
		~D3D11GpuHullProgram();

		/// Gets the vertex shader
		ID3D11HullShader* getHullShader() const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};

	/** 
		Direct3D implementation of low-level geometry programs. 
		Added due to need to accept geometry programs came from other profiles (nvgp4, for example)
	*/
	class D3D11GpuGeometryProgram : public D3D11GpuProgram
	{
	protected:
		ID3D11GeometryShader* mGeometryShader;
	public:
		D3D11GpuGeometryProgram(GpuProgramProfile profile);
		~D3D11GpuGeometryProgram();

		/// Gets the geometry shader
		ID3D11GeometryShader* getGeometryShader(void) const;
	protected:
		void unloadImpl(void);
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	};
}