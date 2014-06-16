#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsGpuProgram.h"

namespace BansheeEngine
{
	class BS_D3D11_EXPORT D3D11GpuProgram : public GpuProgram
	{
		static UINT32 GlobalProgramId;

	public:
		virtual ~D3D11GpuProgram();

		const String& getLanguage() const;

		GpuParamsPtr createParameters();

		/**
		 * @copydoc	GpuProgram::requiresMatrixTranspose
		 */
		virtual bool requiresMatrixTranspose() const { return mColumnMajorMatrices; }

		/** Sets whether matrix packing in column-major order. */
		void setColumnMajorMatrices(bool columnMajor) { mColumnMajorMatrices = columnMajor; }
		/** Gets whether matrix packed in column-major order. */
		bool getColumnMajorMatrices() const { return mColumnMajorMatrices; }
		/** Sets whether backwards compatibility is enabled. */
		void setEnableBackwardsCompatibility(bool enableBackwardsCompatibility) { mEnableBackwardsCompatibility = enableBackwardsCompatibility; }
		/** Gets whether backwards compatibility is enabled. */
		bool getEnableBackwardsCompatibility() const { return mEnableBackwardsCompatibility; }

		const HLSLMicroCode& getMicroCode() const { return mMicrocode; }

		VertexDeclarationPtr getInputDeclaration() const { return mInputDeclaration; }
		UINT32 getProgramId() const { return mProgramId; }

	protected:
		D3D11GpuProgram(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired);

		/**
		* @copydoc GpuProgram::initialize_internal()
		*/
		void initialize_internal();

		/**
		* @copydoc GpuProgram::destroy_internal()
		*/
		void destroy_internal();

		/**
		 * @brief	Loads shader from microcode.
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

	class BS_D3D11_EXPORT D3D11GpuVertexProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuVertexProgram();

		ID3D11VertexShader* getVertexShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11GpuVertexProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

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

	class BS_D3D11_EXPORT D3D11GpuFragmentProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuFragmentProgram();

		ID3D11PixelShader* getPixelShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11GpuFragmentProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();
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

	class BS_D3D11_EXPORT D3D11GpuDomainProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuDomainProgram();

		ID3D11DomainShader* getDomainShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11GpuDomainProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

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

	class BS_D3D11_EXPORT D3D11GpuHullProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuHullProgram();

		ID3D11HullShader* getHullShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);
	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11GpuHullProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

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

	class BS_D3D11_EXPORT D3D11GpuGeometryProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuGeometryProgram();

		ID3D11GeometryShader* getGeometryShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11GpuGeometryProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes,
			bool isAdjacencyInfoRequired);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

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

	class BS_D3D11_EXPORT D3D11GpuComputeProgram : public D3D11GpuProgram
	{
	public:
		~D3D11GpuComputeProgram();

		ID3D11ComputeShader* getComputeShader() const;
		void loadFromMicrocode(D3D11Device& device, ID3D10Blob* microcode);

	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11GpuComputeProgram(const String& source, const String& entryPoint,
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes);

		/**
		 * @copydoc GpuProgram::destroy_internal().
		 */
		void destroy_internal();

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