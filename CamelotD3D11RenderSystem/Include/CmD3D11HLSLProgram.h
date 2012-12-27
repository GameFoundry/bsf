#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHighLevelGpuProgram.h"

namespace CamelotEngine
{
	class D3D11HLSLProgram : public HighLevelGpuProgram
	{
	public:
		~D3D11HLSLProgram();

		const String& getLanguage() const;
		bool isSupported() const;

		/** Sets whether matrix packing in column-major order. */ 
		void setColumnMajorMatrices(bool columnMajor) { mColumnMajorMatrices = columnMajor; }
		/** Gets whether matrix packed in column-major order. */
		bool getColumnMajorMatrices() const { return mColumnMajorMatrices; }
		/** Sets whether backwards compatibility is enabled. */ 
		void setEnableBackwardsCompatibility(bool enableBackwardsCompatibility) { mEnableBackwardsCompatibility = enableBackwardsCompatibility; }
		/** Gets whether backwards compatibility is enabled. */
		bool getEnableBackwardsCompatibility() const { return mEnableBackwardsCompatibility; }

		const HLSLMicroCode& getMicroCode() const { return mMicrocode; }
		UINT32 getNumInputs() const { return (UINT32)mInputParameters.size(); }
		UINT32 getNumOutputs() const { return (UINT32)mOutputParameters.size(); }

		const D3D11_SIGNATURE_PARAMETER_DESC& getInputParamDesc(unsigned int index) const { return mInputParameters.at(index); }
		const D3D11_SIGNATURE_PARAMETER_DESC& getOutputParamDesc(unsigned int index) const { return mOutputParameters.at(index); }

	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11HLSLProgram(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

        /**
         * @copydoc GpuProgram::loadFromSource()
         */
        void loadFromSource();

        /**
         * @copydoc GpuProgram::unload_internal()
         */
        void unload_internal();

		/**
         * @copydoc HighLevelGpuProgram::buildConstantDefinitions()
         */
		void buildConstantDefinitions() const;
	private:
		bool mColumnMajorMatrices;
		bool mEnableBackwardsCompatibility;

		HLSLMicroCode mMicrocode;

		struct D3D11_VariableDesc
		{
			String name;
			D3D11_SHADER_TYPE_DESC desc;
		};

		struct D3D11_ShaderBufferDesc
		{
			D3D11_SHADER_BUFFER_DESC desc;
			vector<D3D11_SHADER_VARIABLE_DESC>::type variables;
			vector<D3D11_SHADER_TYPE_DESC>::type variableTypes;
		};

		vector<D3D11_ShaderBufferDesc>::type mShaderBuffers;
		vector<D3D11_SIGNATURE_PARAMETER_DESC>::type mInputParameters;
		vector<D3D11_SIGNATURE_PARAMETER_DESC>::type mOutputParameters;

		vector<HardwareConstantBufferPtr>::type mConstantBuffers;

		/**
		 * @brief	Compiles the shader from source and generates the microcode.
		 */
		ID3DBlob* compileMicrocode();

		/**
		 * @brief	Reflects the microcode and extracts input/output parameters, and constant
		 * 			buffer structures used by the program.
		 */
		void populateParametersAndConstants(ID3DBlob* microcode);

		void populateConstantBufferParameters(ID3D11ShaderReflectionConstantBuffer* bufferReflection);

		void populateParameterDefinition(const D3D11_SHADER_VARIABLE_DESC& paramDesc, const D3D11_SHADER_TYPE_DESC& d3dDesc, GpuConstantDefinition& def) const;

		/**
		 * @brief	Creates constant buffers based on available parameter and constant data.
		 */
		void createConstantBuffers();

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class D3D11HLSLProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}