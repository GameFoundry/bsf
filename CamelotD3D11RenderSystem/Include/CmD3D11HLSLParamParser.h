#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmVertexDeclaration.h"

namespace BansheeEngine
{
	class D3D11HLSLParamParser
	{
	public:
		void parse(ID3DBlob* microcode, GpuParamDesc& desc, VertexDeclaration::VertexElementList* inputParams);

	private:
		void parseBuffer(ID3D11ShaderReflectionConstantBuffer* bufferReflection, GpuParamDesc& desc);
		void parseResource(D3D11_SHADER_INPUT_BIND_DESC& resourceDesc, GpuParamDesc& desc);
		void parseVariable(D3D11_SHADER_TYPE_DESC& varTypeDesc, D3D11_SHADER_VARIABLE_DESC& varDesc, GpuParamDesc& desc, GpuParamBlockDesc& paramBlock);
	};
}