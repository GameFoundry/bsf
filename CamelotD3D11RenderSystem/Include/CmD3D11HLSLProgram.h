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

		const HLSLMicroCode& getMicroCode() const;
		unsigned int getNumInputs() const;
		unsigned int getNumOutputs() const;

		const D3D11_SIGNATURE_PARAMETER_DESC& getInputParamDesc(unsigned int index) const;
		const D3D11_SIGNATURE_PARAMETER_DESC& getOutputParamDesc(unsigned int index) const;

	protected:
		friend class D3D11HLSLProgramFactory;

		D3D11HLSLProgram(const String& source, const String& entryPoint, const String& language, 
			GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired = false);

        ///** Internal load implementation, must be implemented by subclasses.
        //*/
        //void loadFromSource(void);
        ///** Internal method for creating an appropriate low-level program from this
        //high-level program, must be implemented by subclasses. */
        //void createLowLevelImpl(void);
        ///// Internal unload implementation, must be implemented by subclasses
        //void unloadHighLevelImpl(void);

		ID3DBlob* compileMicrocode();

	private:
		bool mColumnMajorMatrices;
		bool mEnableBackwardsCompatibility;
	};
}