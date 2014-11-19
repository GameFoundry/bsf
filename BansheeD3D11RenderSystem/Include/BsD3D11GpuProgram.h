#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuProgram.h"

namespace BansheeEngine
{
	/**
	 * @brief	Abstraction of a DirectX 11 shader object.
	 */
	class BS_D3D11_EXPORT D3D11GpuProgramCore : public GpuProgramCore
	{
	public:
		virtual ~D3D11GpuProgramCore();

		/**
		 * @copydoc	GpuProgramCore::hasColumnMajorMatrices
		 */
		bool hasColumnMajorMatrices() const { return mColumnMajorMatrices; }

		/**
		 * @brief	Returns compiled shader microcode.
		 */
		const HLSLMicroCode& getMicroCode() const { return mMicrocode; }

		/**
		 * @brief	Returns GPU program input declaration. Only relevant for vertex programs.
		 */
		SPtr<VertexDeclarationCore> getInputDeclaration() const { return mInputDeclaration; }

		/**
		 * @brief	Returns unique GPU program ID.
		 */
		UINT32 getProgramId() const { return mProgramId; }

	protected:
		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuProgramCore(const String& source, const String& entryPoint, GpuProgramType gptype,
			GpuProgramProfile profile, bool isAdjacencyInfoRequired);

		/**
		 * @copydoc GpuProgramCore::initialize
		 */
		void initialize();

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @brief	Loads the shader from microcode.
		 */
		virtual void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode) = 0;

		/**
		 * @brief	Compiles the shader from source and generates the microcode.
		 */
		ID3DBlob* compileMicrocode(const String& profile);

		/**
		 * @brief	Reflects the microcode and extracts input/output parameters, and constant
		 * 			buffer structures used by the program.
		 */
		void populateParametersAndConstants(ID3DBlob* microcode);

	protected:
		static UINT32 GlobalProgramId;

		bool mColumnMajorMatrices;
		bool mEnableBackwardsCompatibility;
		UINT32 mProgramId;

		HLSLMicroCode mMicrocode;
		SPtr<VertexDeclarationCore> mInputDeclaration;
	};

	/**
	 * @brief	Implementation of a DX11 vertex shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuVertexProgramCore : public D3D11GpuProgramCore
	{
	public:
		~D3D11GpuVertexProgramCore();

		/**
		 * @brief	Returns internal DX11 vertex shader object.
		 */
		ID3D11VertexShader* getVertexShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuVertexProgramCore(const String& source, const String& entryPoint,
			GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc	D3D11GpuProgramCore::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11VertexShader* mVertexShader;
	};

	/**
	 * @brief	Implementation of a DX11 pixel shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuFragmentProgramCore : public D3D11GpuProgramCore
	{
	public:
		~D3D11GpuFragmentProgramCore();

		/**
		 * @brief	Returns internal DX11 pixel shader object.
		 */
		ID3D11PixelShader* getPixelShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuFragmentProgramCore(const String& source, const String& entryPoint,
			GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc	D3D11GpuProgramCore::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11PixelShader* mPixelShader;
	};

	/**
	 * @brief	Implementation of a DX11 domain shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuDomainProgramCore : public D3D11GpuProgramCore
	{
	public:
		~D3D11GpuDomainProgramCore();

		/**
		 * @brief	Returns internal DX11 domain shader object.
		 */
		ID3D11DomainShader* getDomainShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuDomainProgramCore(const String& source, const String& entryPoint,
			GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc	D3D11GpuProgramCore::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11DomainShader* mDomainShader;
	};

	/**
	 * @brief	Implementation of a DX11 hull shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuHullProgramCore : public D3D11GpuProgramCore
	{
	public:
		~D3D11GpuHullProgramCore();

		/**
		 * @brief	Returns internal DX11 hull shader object.
		 */
		ID3D11HullShader* getHullShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuHullProgramCore(const String& source, const String& entryPoint,
			GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc	D3D11GpuProgramCore::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11HullShader* mHullShader;
	};

	/**
	 * @brief	Implementation of a DX11 geometry shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuGeometryProgramCore : public D3D11GpuProgramCore
	{
	public:
		~D3D11GpuGeometryProgramCore();

		/**
		 * @brief	Returns internal DX11 geometry shader object.
		 */
		ID3D11GeometryShader* getGeometryShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuGeometryProgramCore(const String& source, const String& entryPoint,
			GpuProgramProfile profile, bool isAdjacencyInfoRequired);

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc	D3D11GpuProgramCore::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11GeometryShader* mGeometryShader;
	};

	/**
	 * @brief	Implementation of a DX11 compute shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuComputeProgramCore : public D3D11GpuProgramCore
	{
	public:
		~D3D11GpuComputeProgramCore();

		/**
		 * @brief	Returns internal DX11 compute shader object.
		 */
		ID3D11ComputeShader* getComputeShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgramCore::GpuProgramCore
		 */
		D3D11GpuComputeProgramCore(const String& source, const String& entryPoint,
			GpuProgramProfile profile);

		/**
		 * @copydoc GpuProgramCore::destroy
		 */
		void destroy();

		/**
		 * @copydoc	D3D11GpuProgramCore::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11ComputeShader* mComputeShader;
	};
}