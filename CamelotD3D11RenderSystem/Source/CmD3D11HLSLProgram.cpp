#include "CmD3D11HLSLProgram.h"
#include "CmRenderSystemManager.h"
#include "CmRenderSystem.h"
#include "CmException.h"

namespace CamelotEngine
{
	D3D11HLSLProgram::D3D11HLSLProgram(const String& source, const String& entryPoint, const String& language, 
		GpuProgramType gptype, GpuProgramProfile profile, bool isAdjacencyInfoRequired)
		: HighLevelGpuProgram(source, entryPoint, language, gptype, profile, isAdjacencyInfoRequired),
		mColumnMajorMatrices(true), mEnableBackwardsCompatibility(false)
	{
	}

	D3D11HLSLProgram::~D3D11HLSLProgram()
	{
		unload();
	}

	const String& D3D11HLSLProgram::getLanguage() const
	{
		static String name = "hlsl";

		return name;
	}

	bool D3D11HLSLProgram::isSupported() const
	{
		RenderSystem* rs = RenderSystemManager::getActive();

		return rs->getCapabilities_internal()->isShaderProfileSupported(getSyntaxCode()) && HighLevelGpuProgram::isSupported();
	}

	ID3DBlob* D3D11HLSLProgram::compileMicrocode()
	{
		// TODO - Preprocessor defines aren't supported

		UINT compileFlags = 0;
#if defined(CM_DEBUG_MODE)
		compileFlags |= D3DCOMPILE_DEBUG;
		compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		if (mColumnMajorMatrices)
			compileFlags |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
		else
			compileFlags |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;

		if (mEnableBackwardsCompatibility)
			compileFlags |= D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY;

		ID3DBlob* microCode = nullptr;
		ID3DBlob* errors = nullptr;

		HRESULT hr = D3DCompile(
			mSource.c_str(),	// [in] Pointer to the shader in memory. 
			mSource.size(),		// [in] Size of the shader in memory.  
			nullptr,			// [in] The name of the file that contains the shader code. 
			nullptr,			// [in] Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
			nullptr,			// [in] Optional. Pointer to an ID3DInclude Interface interface for handling include files. Setting this to NULL will cause a compile error if a shader contains a #include. 
			mEntryPoint.c_str(),// [in] Name of the shader-entrypoint function where shader execution begins. 
			mSyntaxCode.c_str(),// [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
			compileFlags,		// [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
			0,					// [in] Effect compile flags
			&microCode,			// [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
			&errors				// [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
			);

		if (FAILED(hr))
		{
			String message = "Cannot assemble D3D11 high-level shader. Errors:\n" +
				String(static_cast<const char*>(errors->GetBufferPointer()));

			SAFE_RELEASE(errors);
			CM_EXCEPT(RenderingAPIException, message);
		}

		SAFE_RELEASE(errors);

		return microCode;
	}

	const HLSLMicroCode& D3D11HLSLProgram::getMicroCode() const
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	unsigned int D3D11HLSLProgram::getNumInputs() const
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	unsigned int D3D11HLSLProgram::getNumOutputs() const
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	const D3D11_SIGNATURE_PARAMETER_DESC& D3D11HLSLProgram::getInputParamDesc(unsigned int index) const
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}

	const D3D11_SIGNATURE_PARAMETER_DESC& D3D11HLSLProgram::getOutputParamDesc(unsigned int index) const
	{
		CM_EXCEPT(NotImplementedException, "Not implemented");
	}
}