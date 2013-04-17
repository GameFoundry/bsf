#pragma once

#include "CmD3D9Prerequisites.h"

namespace CamelotFramework
{
	class D3D9HLSLParamParser
	{
	public:
		D3D9HLSLParamParser(LPD3DXCONSTANTTABLE constTable)
			:mpConstTable(constTable)
		{ }

		GpuParamDesc buildParameterDescriptions();

	private:
		void processParameter(GpuParamBlockDesc& blockDesc, D3DXHANDLE parent, String prefix, UINT32 index);
		void populateParamMemberDesc(GpuParamDataDesc& memberDesc, D3DXCONSTANT_DESC& d3dDesc);

	private:
		LPD3DXCONSTANTTABLE mpConstTable;
		GpuParamDesc mParamDesc;
	};

	GpuParamDesc D3D9HLSLParamParser::buildParameterDescriptions()
	{
		// Derive parameter names from const table
		assert(mpConstTable && "Program not loaded!");

		// Get contents of the constant table
		D3DXCONSTANTTABLE_DESC desc;
		HRESULT hr = mpConstTable->GetDesc(&desc);

		if (FAILED(hr))
			CM_EXCEPT(InternalErrorException, "Cannot retrieve constant descriptions from HLSL program.");

		// DX9 has no concept of parameter blocks so we just put all members in one global block
		String name = "CM_INTERNAL_Globals";
		mParamDesc.paramBlocks.insert(std::make_pair(name, GpuParamBlockDesc()));
		GpuParamBlockDesc& blockDesc = mParamDesc.paramBlocks[name];
		blockDesc.name = name;
		blockDesc.slot = 0;
		blockDesc.blockSize = 0;
		blockDesc.isShareable = false;

		// Iterate over the constants
		for (UINT32 i = 0; i < desc.Constants; ++i)
		{
			// Recursively descend through the structure levels
			processParameter(blockDesc, NULL, "", i);
		}

		return mParamDesc;
	}

	void D3D9HLSLParamParser::processParameter(GpuParamBlockDesc& blockDesc, D3DXHANDLE parent, String prefix, UINT32 index)
	{
		D3DXHANDLE hConstant = mpConstTable->GetConstant(parent, index);

		// Since D3D HLSL doesn't deal with naming of array and struct parameters
		// automatically, we have to do it by hand

		D3DXCONSTANT_DESC desc;
		UINT32 numParams = 1;
		HRESULT hr = mpConstTable->GetConstantDesc(hConstant, &desc, &numParams);
		if (FAILED(hr))
		{
			CM_EXCEPT(InternalErrorException, "Cannot retrieve constant description from HLSL program.");
		}

		String paramName = desc.Name;
		// trim the odd '$' which appears at the start of the names in HLSL
		if (paramName.at(0) == '$')
			paramName.erase(paramName.begin());

		// Also trim the '[0]' suffix if it exists, we will add our own indexing later
		if (StringUtil::endsWith(paramName, "[0]", false))
			paramName.erase(paramName.size() - 3);

		if (desc.Class == D3DXPC_STRUCT)
		{
			// work out a new prefix for nested members, if it's an array, we need an index
			prefix = prefix + paramName + ".";
			// Cascade into struct
			for (UINT32 i = 0; i < desc.StructMembers; ++i)
			{
				processParameter(blockDesc, hConstant, prefix, i);
			}
		}
		else
		{
			// Process params
			if (desc.Type == D3DXPT_FLOAT || desc.Type == D3DXPT_INT || desc.Type == D3DXPT_BOOL)
			{
				GpuParamDataDesc memberDesc;
				memberDesc.gpuMemOffset = desc.RegisterIndex;
				memberDesc.cpuMemOffset = blockDesc.blockSize;
				memberDesc.paramBlockSlot = blockDesc.slot;
				memberDesc.arraySize = 1;

				String name = prefix + paramName;
				memberDesc.name = name;

				populateParamMemberDesc(memberDesc, desc);
				mParamDesc.params.insert(std::make_pair(name, memberDesc));

				blockDesc.blockSize += memberDesc.arrayElementStride * memberDesc.arraySize;
			}
			else if(desc.Type == D3DXPT_SAMPLER1D || desc.Type == D3DXPT_SAMPLER2D || desc.Type == D3DXPT_SAMPLER3D || desc.Type == D3DXPT_SAMPLERCUBE)
			{
				GpuParamObjectDesc samplerDesc;
				samplerDesc.name = paramName;
				samplerDesc.slot = desc.RegisterIndex;

				GpuParamObjectDesc textureDesc;
				textureDesc.name = paramName;
				textureDesc.slot = desc.RegisterIndex;

				switch(desc.Type)
				{
				case D3DXPT_SAMPLER1D:
					samplerDesc.type = GPOT_SAMPLER1D;
					textureDesc.type = GPOT_TEXTURE1D;
					break;
				case D3DXPT_SAMPLER2D:
					samplerDesc.type = GPOT_SAMPLER2D;
					textureDesc.type = GPOT_TEXTURE2D;
					break;
				case D3DXPT_SAMPLER3D:
					samplerDesc.type = GPOT_SAMPLER3D;
					textureDesc.type = GPOT_TEXTURE3D;
					break;
				case D3DXPT_SAMPLERCUBE:
					samplerDesc.type = GPOT_SAMPLERCUBE;
					textureDesc.type = GPOT_TEXTURECUBE;
					break;
				default:
					CM_EXCEPT(InternalErrorException, "Invalid sampler type: " + toString(desc.Type) + " for parameter " + paramName);
				}

				mParamDesc.samplers.insert(std::make_pair(paramName, samplerDesc));
				mParamDesc.textures.insert(std::make_pair(paramName, textureDesc));
			}
			else
			{
				CM_EXCEPT(InternalErrorException, "Invalid shader parameter type: " + toString(desc.Type) + " for parameter " + paramName);
			}
		}
	}

	void D3D9HLSLParamParser::populateParamMemberDesc(GpuParamDataDesc& memberDesc, D3DXCONSTANT_DESC& d3dDesc)
	{
		memberDesc.arraySize = d3dDesc.Elements;
		switch(d3dDesc.Type)
		{
		case D3DXPT_INT:
			switch(d3dDesc.Columns)
			{
			case 1:
				memberDesc.type = GPDT_INT1;
				memberDesc.elementSize = 4;
				memberDesc.arrayElementStride = 4;
				break;
			case 2:
				memberDesc.type = GPDT_INT2;
				memberDesc.elementSize = 4;
				memberDesc.arrayElementStride = 4;
				break;
			case 3:
				memberDesc.type = GPDT_INT3;
				memberDesc.elementSize = 4;
				memberDesc.arrayElementStride = 4;
				break;
			case 4:
				memberDesc.type = GPDT_INT4;
				memberDesc.elementSize = 4;
				memberDesc.arrayElementStride = 4;
				break;
			} // columns
			break;
		case D3DXPT_FLOAT:
			switch(d3dDesc.Class)
			{
			case D3DXPC_MATRIX_COLUMNS:
			case D3DXPC_MATRIX_ROWS:
				{
					int firstDim, secondDim;
					firstDim = d3dDesc.RegisterCount / d3dDesc.Elements;

					if (d3dDesc.Class == D3DXPC_MATRIX_ROWS)
						secondDim = d3dDesc.Columns;
					else
						secondDim = d3dDesc.Rows;

					switch(firstDim)
					{
					case 2:
						switch(secondDim)
						{
						case 2:
							memberDesc.type = GPDT_MATRIX_2X2;
							memberDesc.elementSize = 8; // HLSL always packs
							memberDesc.arrayElementStride = 8;
							break;
						case 3:
							memberDesc.type = GPDT_MATRIX_2X3;
							memberDesc.elementSize = 8; // HLSL always packs
							memberDesc.arrayElementStride = 8;
							break;
						case 4:
							memberDesc.type = GPDT_MATRIX_2X4;
							memberDesc.elementSize = 8; 
							memberDesc.arrayElementStride = 8;
							break;
						} // columns
						break;
					case 3:
						switch(secondDim)
						{
						case 2:
							memberDesc.type = GPDT_MATRIX_3X2;
							memberDesc.elementSize = 12; // HLSL always packs
							memberDesc.arrayElementStride = 12;
							break;
						case 3:
							memberDesc.type = GPDT_MATRIX_3X3;
							memberDesc.elementSize = 12; // HLSL always packs
							memberDesc.arrayElementStride = 12;
							break;
						case 4:
							memberDesc.type = GPDT_MATRIX_3X4;
							memberDesc.elementSize = 12; 
							memberDesc.arrayElementStride = 12;
							break;
						} // columns
						break;
					case 4:
						switch(secondDim)
						{
						case 2:
							memberDesc.type = GPDT_MATRIX_4X2;
							memberDesc.elementSize = 16; // HLSL always packs
							memberDesc.arrayElementStride = 16;
							break;
						case 3:
							memberDesc.type = GPDT_MATRIX_4X3;
							memberDesc.elementSize = 16; // HLSL always packs
							memberDesc.arrayElementStride = 16;
							break;
						case 4:
							memberDesc.type = GPDT_MATRIX_4X4;
							memberDesc.elementSize = 16; 
							memberDesc.arrayElementStride = 16;
							break;
						} // secondDim
						break;

					} // firstDim
				}
				break;
			case D3DXPC_SCALAR:
			case D3DXPC_VECTOR:
				switch(d3dDesc.Columns)
				{
				case 1:
					memberDesc.type = GPDT_FLOAT1;
					memberDesc.elementSize = 4;
					memberDesc.arrayElementStride = 4;
					break;
				case 2:
					memberDesc.type = GPDT_FLOAT2;
					memberDesc.elementSize = 4;
					memberDesc.arrayElementStride = 4;
					break;
				case 3:
					memberDesc.type = GPDT_FLOAT3;
					memberDesc.elementSize = 4;
					memberDesc.arrayElementStride = 4;
					break;
				case 4:
					memberDesc.type = GPDT_FLOAT4;
					memberDesc.elementSize = 4;
					memberDesc.arrayElementStride = 4;
					break;
				} // columns
				break;
			}
			break;
		case D3DXPT_BOOL:
			memberDesc.type = GPDT_BOOL;
			memberDesc.elementSize = 1;
			memberDesc.arrayElementStride = 1;
			break;
		default:
			break;
		};
	}
}