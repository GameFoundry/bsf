//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuProgram.h"

namespace BansheeEngine
{
	/**
	 * @brief	Abstraction of a DirectX 11 shader object.
	 */
	class BS_D3D11_EXPORT D3D11GpuProgram : public GpuProgram
	{
	public:
		virtual ~D3D11GpuProgram();

		/**
		 * @copydoc	GpuProgram::getLanguage
		 */
		const String& getLanguage() const;

		/**
		 * @copydoc	GpuProgram::createParameters
		 */
		GpuParamsPtr createParameters();

		/**
		 * @copydoc	GpuProgram::requiresMatrixTranspose
		 */
		virtual bool requiresMatrixTranspose() const { return mColumnMajorMatrices; }

		/**
		 * @brief	Returns compiled shader microcode.
		 */
		const HLSLMicroCode& getMicroCode() const { return mMicrocode; }

		/**
		 * @brief	Returns GPU program input declaration. Only relevant for vertex programs.
		 */
		VertexDeclarationPtr getInputDeclaration() const { return mInputDeclaration; }

		/**
		 * @brief	Returns unique GPU program ID.
		 */
		UINT32 getProgramId() const { return mProgramId; }

	protected:
		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuProgram(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired);

		/**
		 * @copydoc GpuProgram::initialize_internal
		 */
		void initialize_internal();

		/**
		 * @copydoc GpuProgram::destroy_internal
		 */
		void destroy_internal();

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
		VertexDeclarationPtr mInputDeclaration;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Implementation of a DX11 vertex shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuVertexProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuVertexProgram();

		/**
		 * @brief	Returns internal DX11 vertex shader object.
		 */
		ID3D11VertexShader* getVertexShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuVertexProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D11GpuProgram::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11VertexShader* mVertexShader;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuVertexProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Implementation of a DX11 pixel shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuFragmentProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuFragmentProgram();

		/**
		 * @brief	Returns internal DX11 pixel shader object.
		 */
		ID3D11PixelShader* getPixelShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuFragmentProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D11GpuProgram::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11PixelShader* mPixelShader;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuFragmentProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Implementation of a DX11 domain shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuDomainProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuDomainProgram();

		/**
		 * @brief	Returns internal DX11 domain shader object.
		 */
		ID3D11DomainShader* getDomainShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuDomainProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D11GpuProgram::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11DomainShader* mDomainShader;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuDomainProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Implementation of a DX11 hull shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuHullProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuHullProgram();

		/**
		 * @brief	Returns internal DX11 hull shader object.
		 */
		ID3D11HullShader* getHullShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuHullProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D11GpuProgram::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11HullShader* mHullShader;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuHullProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Implementation of a DX11 geometry shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuGeometryProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuGeometryProgram();

		/**
		 * @brief	Returns internal DX11 geometry shader object.
		 */
		ID3D11GeometryShader* getGeometryShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuGeometryProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes,
			bool isAdjacencyInfoRequired);

		/**
		 * @copydoc GpuProgram::destroy_internal
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D11GpuProgram::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11GeometryShader* mGeometryShader;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuGeometryProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	/**
	 * @brief	Implementation of a DX11 compute shader.
	 */
	class BS_D3D11_EXPORT D3D11GpuComputeProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuComputeProgram();

		/**
		 * @brief	Returns internal DX11 compute shader object.
		 */
		ID3D11ComputeShader* getComputeShader() const;

	protected:
		friend class D3D11HLSLProgramFactory;

		/**
		 * @copydoc	GpuProgram::GpuProgram
		 */
		D3D11GpuComputeProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal
		 */
		void destroy_internal();

		/**
		 * @copydoc	D3D11GpuProgram::loadFromMicrocode
		 */
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		ID3D11ComputeShader* mComputeShader;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11GpuComputeProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}