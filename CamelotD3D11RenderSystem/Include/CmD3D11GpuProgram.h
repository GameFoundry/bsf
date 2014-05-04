#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuProgram.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11GpuProgram : public GpuProgram
	{
	public:
		virtual ~D3D11GpuProgram();

		bool isSupported() const;
	protected:
		D3D11GpuProgram(GpuProgramType type, GpuProgramProfile profile);

		/**
		 * @brief	Loads shader from microcode.
		 */
		virtual void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode) = 0;
	};

	class CM_D3D11_EXPORT D3D11GpuVertexProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuVertexProgram();

		ID3D11VertexShader* getVertexShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11GpuProgramManager;

		D3D11GpuVertexProgram(GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

	protected:
		ID3D11VertexShader* mVertexShader;
	};

	class CM_D3D11_EXPORT D3D11GpuFragmentProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuFragmentProgram();

		ID3D11PixelShader* getPixelShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11GpuProgramManager;

		D3D11GpuFragmentProgram(GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();
	protected:
		ID3D11PixelShader* mPixelShader;
	};

	class CM_D3D11_EXPORT D3D11GpuDomainProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuDomainProgram();

		ID3D11DomainShader* getDomainShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11GpuProgramManager;

		D3D11GpuDomainProgram(GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

	protected:
		ID3D11DomainShader* mDomainShader;
	};

	class CM_D3D11_EXPORT D3D11GpuHullProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuHullProgram();

		ID3D11HullShader* getHullShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11GpuProgramManager;

		D3D11GpuHullProgram(GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

	protected:
		ID3D11HullShader* mHullShader;
	};

	class CM_D3D11_EXPORT D3D11GpuGeometryProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuGeometryProgram();

		ID3D11GeometryShader* getGeometryShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		friend class D3D11GpuProgramManager;

		D3D11GpuGeometryProgram(GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

	protected:
		ID3D11GeometryShader* mGeometryShader;
	};

	class CM_D3D11_EXPORT D3D11GpuComputeProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuComputeProgram();

		ID3D11ComputeShader* getComputeShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		friend class D3D11GpuProgramManager;

		D3D11GpuComputeProgram(GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

	protected:
		ID3D11ComputeShader* mComputeShader;
	};
}