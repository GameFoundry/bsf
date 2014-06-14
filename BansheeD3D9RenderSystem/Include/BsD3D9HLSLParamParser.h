#pragma once

#include "BsD3D9Prerequisites.h"
#include "BsD3D9GpuProgram.h"

namespace BansheeEngine
{
	class D3D9HLSLParamParser
	{
	public:
		D3D9HLSLParamParser(LPD3DXCONSTANTTABLE constTable, const Vector<D3D9EmulatedParamBlock>& blocks)
			:mpConstTable(constTable), mBlocks(blocks)
		{ }

		GpuParamDesc buildParameterDescriptions();

	private:
		void processParameter(GpuParamBlockDesc& blockDesc, const String& paramName, D3DXHANDLE constant,
			String prefix, UINT32 index);
		void populateParamMemberDesc(GpuParamDataDesc& memberDesc, D3DXCONSTANT_DESC& d3dDesc);

		String getParamName(D3DXHANDLE constant);

	private:
		LPD3DXCONSTANTTABLE mpConstTable;
		Vector<D3D9EmulatedParamBlock> mBlocks;
		GpuParamDesc mParamDesc;
	};

	String D3D9HLSLParamParser::getParamName(D3DXHANDLE constant)
	{
		D3DXCONSTANT_DESC desc;
		UINT32 numParams = 1;
		HRESULT hr = mpConstTable->GetConstantDesc(constant, &desc, &numParams);
		if (FAILED(hr))
		{
			BS_EXCEPT(InternalErrorException, "Cannot retrieve constant description from HLSL program.");
		}

		String paramName = desc.Name;
		// trim the odd '$' which appears at the start of the names in HLSL
		if (paramName.at(0) == '$')
			paramName.erase(paramName.begin());

		// Also trim the '[0]' suffix if it exists, we will add our own indexing later
		if (StringUtil::endsWith(paramName, "[0]", false))
			paramName.erase(paramName.size() - 3);

		return paramName;
	}

	GpuParamDesc D3D9HLSLParamParser::buildParameterDescriptions()
	{
		// Derive parameter names from const table
		assert(mpConstTable && "Program not loaded!");

		// Get contents of the constant table
		D3DXCONSTANTTABLE_DESC desc;
		HRESULT hr = mpConstTable->GetDesc(&desc);

		if (FAILED(hr))
			BS_EXCEPT(InternalErrorException, "Cannot retrieve constant descriptions from HLSL program.");

		// DX9 has no concept of parameter blocks so we emulate them if needed
		String name = "BS_INTERNAL_Globals";
		mParamDesc.paramBlocks.insert(std::make_pair(name, GpuParamBlockDesc()));
		GpuParamBlockDesc& globalBlockDesc = mParamDesc.paramBlocks[name];
		globalBlockDesc.name = name;
		globalBlockDesc.slot = 0;
		globalBlockDesc.blockSize = 0;
		globalBlockDesc.isShareable = false;

		UnorderedMap<String, String> nonGlobalBlocks;
		UINT32 curSlot = 1;
		for (auto& block : mBlocks)
		{
			mParamDesc.paramBlocks.insert(std::make_pair(block.blockName, GpuParamBlockDesc()));
			GpuParamBlockDesc& blockDesc = mParamDesc.paramBlocks[block.blockName];
			globalBlockDesc.name = block.blockName;
			globalBlockDesc.slot = curSlot++;
			globalBlockDesc.blockSize = 0;
			globalBlockDesc.isShareable = true;

			for (auto& fieldName : block.paramNames)
			{
				nonGlobalBlocks.insert(std::make_pair(fieldName, block.blockName));
			}
		}

		// Iterate over the constants
		for (UINT32 i = 0; i < desc.Constants; ++i)
		{
			D3DXHANDLE constantHandle = mpConstTable->GetConstant(NULL, i);
			String paramName = getParamName(constantHandle);

			// Recursively descend through the structure levels
			auto findIter = nonGlobalBlocks.find(paramName);
			if (findIter == nonGlobalBlocks.end())
				processParameter(globalBlockDesc, paramName, constantHandle, "", i);
			else
				processParameter(mParamDesc.paramBlocks[findIter->second], paramName, constantHandle, "", i);
		}

		return mParamDesc;
	}

	void D3D9HLSLParamParser::processParameter(GpuParamBlockDesc& blockDesc, const String& paramName, D3DXHANDLE constant, String prefix, UINT32 index)
	{
		// Since D3D HLSL doesn't deal with naming of array and struct parameters
		// automatically, we have to do it by hand

		D3DXCONSTANT_DESC desc;
		UINT32 numParams = 1;
		HRESULT hr = mpConstTable->GetConstantDesc(constant, &desc, &numParams);
		if (FAILED(hr))
		{
			BS_EXCEPT(InternalErrorException, "Cannot retrieve constant description from HLSL program.");
		}

		if (desc.Class == D3DXPC_STRUCT)
		{
			// work out a new prefix for nested members, if it's an array, we need an index
			prefix = prefix + paramName + ".";
			// Cascade into struct
			for (UINT32 i = 0; i < desc.StructMembers; ++i)
			{
				D3DXHANDLE childHandle = mpConstTable->GetConstant(constant, i);
				String childParamName = getParamName(childHandle);

				processParameter(blockDesc, childParamName, childHandle, prefix, i);
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
					BS_EXCEPT(InternalErrorException, "Invalid sampler type: " + toString(desc.Type) + " for parameter " + paramName);
				}

				mParamDesc.samplers.insert(std::make_pair(paramName, samplerDesc));
				mParamDesc.textures.insert(std::make_pair(paramName, textureDesc));
			}
			else
			{
				BS_EXCEPT(InternalErrorException, "Invalid shader parameter type: " + toString(desc.Type) + " for parameter " + paramName);
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
					int firstActualDim; // Actual size might be less than requested because of optimization, we need to know both
					firstActualDim = d3dDesc.RegisterCount / d3dDesc.Elements;

					if (d3dDesc.Class == D3DXPC_MATRIX_ROWS)
					{
						firstDim = d3dDesc.Rows;
						secondDim = d3dDesc.Columns;
					}
					else
					{
						firstDim = d3dDesc.Columns;
						secondDim = d3dDesc.Rows;
					}

					switch (firstActualDim)
					{
					case 2:
						switch(secondDim)
						{
						case 2:
							memberDesc.elementSize = 8; // HLSL always packs
							memberDesc.arrayElementStride = 8;
							break;
						case 3:
							memberDesc.elementSize = 8; // HLSL always packs
							memberDesc.arrayElementStride = 8;
							break;
						case 4:
							memberDesc.elementSize = 8; 
							memberDesc.arrayElementStride = 8;
							break;
						}
						break;
					case 3:
						switch(secondDim)
						{
						case 2:
							memberDesc.elementSize = 12; // HLSL always packs
							memberDesc.arrayElementStride = 12;
							break;
						case 3:
							memberDesc.elementSize = 12; // HLSL always packs
							memberDesc.arrayElementStride = 12;
							break;
						case 4:
							memberDesc.elementSize = 12; 
							memberDesc.arrayElementStride = 12;
							break;
						}
						break;
					case 4:
						switch(secondDim)
						{
						case 2:
							memberDesc.elementSize = 16; // HLSL always packs
							memberDesc.arrayElementStride = 16;
							break;
						case 3:
							memberDesc.elementSize = 16; // HLSL always packs
							memberDesc.arrayElementStride = 16;
							break;
						case 4:
							memberDesc.elementSize = 16; 
							memberDesc.arrayElementStride = 16;
							break;
						}
						break;

					}

					switch (firstDim)
					{
					case 2:
						switch (secondDim)
						{
						case 2:
							memberDesc.type = GPDT_MATRIX_2X2;
							break;
						case 3:
							memberDesc.type = GPDT_MATRIX_2X3;
							break;
						case 4:
							memberDesc.type = GPDT_MATRIX_2X4;
							break;
						} 
						break;
					case 3:
						switch (secondDim)
						{
						case 2:
							memberDesc.type = GPDT_MATRIX_3X2;
							break;
						case 3:
							memberDesc.type = GPDT_MATRIX_3X3;
							break;
						case 4:
							memberDesc.type = GPDT_MATRIX_3X4;
							break;
						} 
						break;
					case 4:
						switch (secondDim)
						{
						case 2:
							memberDesc.type = GPDT_MATRIX_4X2;
							break;
						case 3:
							memberDesc.type = GPDT_MATRIX_4X3;
							break;
						case 4:
							memberDesc.type = GPDT_MATRIX_4X4;
							break;
						}
						break;
					}
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