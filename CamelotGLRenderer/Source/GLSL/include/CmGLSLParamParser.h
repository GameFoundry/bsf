#pragma once

#include "CmGLPrerequisites.h"
#include "CmVertexDeclaration.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmGpuParamDesc.h"

namespace CamelotFramework
{
	struct GLSLAttribute
	{
		GLSLAttribute(const String& name, VertexElementSemantic semantic)
			:mName(name), mSemantic(semantic)
		{ }

		INT32 matchesName(const String& name);
		VertexElementSemantic getSemantic() const { return mSemantic; }

	private:
		String mName;
		VertexElementSemantic mSemantic;
	};

	INT32 GLSLAttribute::matchesName(const String& name)
	{
		if(name.length() >= mName.length())
		{
			if(name.substr(0, mName.length()) == mName)
			{
				String indexStr = name.substr(mName.length(), name.length());
				return parseUnsignedInt(indexStr, 0);
			}
		}

		return -1;
	}

	struct GLSLParamArrayData
	{
		vector<UINT32>::type arrayIndices;
	};

	class GLSLParamParser
	{
	public:
		void buildUniformDescriptions(GLuint glProgram, GpuParamDesc& returnParamDesc);
		void buildVertexDeclaration(GLuint glProgram, VertexDeclaration& declaration);

	private:
		void determineParamInfo(GpuParamDataDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex);

		/**
		* @brief	GLSL has no concept of semantics, so we require all shaders to use specific names for attributes
		* 			so that we know what they are used for.
		*
		* @return	true if it succeeds, false if it fails.
		*/
		bool attribNameToElementSemantic(const String& name, VertexElementSemantic& semantic, UINT16& index);
		VertexElementType glTypeToAttributeType(GLenum glType);
	};

	void GLSLParamParser::buildVertexDeclaration(GLuint glProgram, VertexDeclaration& declaration)
	{
		GLint numAttributes = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_ATTRIBUTES, &numAttributes);

		GLint maxNameSize = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxNameSize);

		GLchar* attributeName = (GLchar*)cm_alloc<ScratchAlloc>(sizeof(GLchar) * maxNameSize);

		for(GLint i = 0; i < numAttributes; i++)
		{
			GLint attribSize = 0;
			GLenum attribType = 0;
			glGetActiveAttrib(glProgram, i, maxNameSize, nullptr, &attribSize, &attribType, attributeName);

			VertexElementSemantic semantic = VES_POSITION;
			UINT16 index = 0;
			if(attribNameToElementSemantic(attributeName, semantic, index))
			{
				VertexElementType type = glTypeToAttributeType(attribType);

				declaration.addElement(0, i, type, semantic, index);
			}
			else
			{
				LOGWRN("Cannot determine vertex input attribute type for attribute: " + String(attributeName));
			}
		}

		cm_free<ScratchAlloc>(attributeName);
	}

	VertexElementType GLSLParamParser::glTypeToAttributeType(GLenum glType)
	{
		switch(glType)
		{
		case GL_FLOAT:
			return VET_FLOAT1;
		case GL_FLOAT_VEC2:
			return VET_FLOAT2;
		case GL_FLOAT_VEC3:
			return VET_FLOAT3;
		case GL_FLOAT_VEC4:
			return VET_FLOAT4;
		default:
			CM_EXCEPT(NotImplementedException, "OpenGL render system currently only supports float parameters.");
		}
	}

	bool GLSLParamParser::attribNameToElementSemantic(const String& name, VertexElementSemantic& semantic, UINT16& index)
	{
		static GLSLAttribute attributes[] = 
		{
			GLSLAttribute("cm_position", VES_POSITION),
			GLSLAttribute("cm_normal", VES_NORMAL),
			GLSLAttribute("cm_tangent", VES_TANGENT),
			GLSLAttribute("cm_bitangent", VES_BITANGENT),
			GLSLAttribute("cm_texcoord", VES_TEXCOORD),
			GLSLAttribute("cm_color", VES_COLOR),
			GLSLAttribute("cm_blendweights", VES_BLEND_WEIGHTS),
			GLSLAttribute("cm_blendindices", VES_BLEND_INDICES)
		};

		static const UINT32 numAttribs = sizeof(attributes) / sizeof(attributes[0]);

		for(UINT32 i = 0; i < numAttribs; i++)
		{
			INT32 attribIndex = attributes[i].matchesName(name);
			if(attribIndex != -1)
			{
				index = attribIndex;
				semantic = attributes[i].getSemantic();
				return true;
			}
		}

		return false;
	}

	void GLSLParamParser::buildUniformDescriptions(GLuint glProgram, GpuParamDesc& returnParamDesc)
	{
		// scan through the active uniforms and add them to the reference list
		GLint maxBufferSize = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxBufferSize);

		GLint maxBlockNameBufferSize = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxBlockNameBufferSize);

		if(maxBlockNameBufferSize > maxBufferSize)
			maxBufferSize = maxBlockNameBufferSize;

		GLchar* uniformName = (GLchar*)cm_alloc<ScratchAlloc>(sizeof(GLchar) * maxBufferSize);

		GpuParamBlockDesc newGlobalBlockDesc;
		newGlobalBlockDesc.slot = 0;
		newGlobalBlockDesc.name = "CM_INTERNAL_Globals";
		newGlobalBlockDesc.blockSize = 0;
		newGlobalBlockDesc.isShareable = false;

		UINT32 textureSlot = 0;

		returnParamDesc.paramBlocks[newGlobalBlockDesc.name] = newGlobalBlockDesc;
		GpuParamBlockDesc& globalBlockDesc = returnParamDesc.paramBlocks[newGlobalBlockDesc.name];

		GLint uniformBlockCount = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);

		map<UINT32, String>::type blockSlotToName;
		set<String>::type blockNames;
		for (GLuint index = 0; index < (GLuint)uniformBlockCount; index++)
		{
			GLsizei unusedSize = 0;
			glGetActiveUniformBlockName(glProgram, index, maxBufferSize, &unusedSize, uniformName);

			GpuParamBlockDesc newBlockDesc;
			newBlockDesc.slot = index + 1;
			newBlockDesc.name = uniformName;
			newBlockDesc.blockSize = 0;
			newBlockDesc.isShareable = true;

			returnParamDesc.paramBlocks[newBlockDesc.name] = newBlockDesc;
			blockSlotToName.insert(std::make_pair(newBlockDesc.slot, newBlockDesc.name));
			blockNames.insert(newBlockDesc.name);
		}

		map<String, UINT32>::type foundFirstArrayIndex; 
		map<String, GpuParamDataDesc>::type foundStructs;

		// get the number of active uniforms
		GLint uniformCount = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORMS, &uniformCount);

		// Loop over each of the active uniforms, and add them to the reference container
		// only do this for user defined uniforms, ignore built in gl state uniforms
		for (GLuint index = 0; index < (GLuint)uniformCount; index++)
		{
			GLsizei arraySize = 0;
			glGetActiveUniformName(glProgram, index, maxBufferSize, &arraySize, uniformName);

			String paramName = String(uniformName);

			// Naming rules and packing rules used here are described in
			// OpenGL Core Specification 2.11.4

			// Check if parameter is a part of a struct
			vector<String>::type nameElements = StringUtil::tokenise(paramName, ".");

			bool inStruct = false;
			String structName;
			if(nameElements.size() > 1)
			{
				auto uniformBlockFind = blockNames.find(nameElements[0]);

				// Check if the name is not a struct, and instead a Uniform block namespace
				if(uniformBlockFind != blockNames.end())
				{
					// Possibly it's a struct inside a named uniform block
					if(nameElements.size() > 2)
					{
						inStruct = true;
						structName = nameElements[1];
					}
				}
				else
				{
					inStruct = true;
					structName = nameElements[0];
				}
			}
			
			String cleanParamName = paramName; // Param name without array indexes

			// Check if the parameter is in an array
			UINT32 arrayIdx = 0;
			bool isInArray = false;
			if(inStruct)
			{
				// If the uniform name has a "[" in it then its an array element uniform.
				String::size_type arrayStart = structName.find("[");
				String::size_type arrayEnd = structName.find("]");
				if (arrayStart != String::npos)
				{
					String strArrIdx = structName.substr(arrayStart + 1, arrayEnd - (arrayStart + 1));
					arrayIdx = parseUnsignedInt(strArrIdx, 0);
					isInArray = true;

					structName = structName.substr(0, arrayStart);
				}
			}
			else
			{
				// If the uniform name has a "[" in it then its an array element uniform.
				String::size_type arrayStart = cleanParamName.find("[");
				String::size_type arrayEnd = cleanParamName.find("]");
				if (arrayStart != String::npos)
				{
					String strArrIdx = cleanParamName.substr(arrayStart + 1, arrayEnd - (arrayStart + 1));
					arrayIdx = parseUnsignedInt(strArrIdx, 0);
					isInArray = true;

					cleanParamName = cleanParamName.substr(0, arrayStart);
				}
			}

			if(inStruct)	
			{
				// OpenGL makes struct management really difficult, which is why I have given up on implementing this so far
				// Some of the issues I encountered:
				//  - Elements will be optimized out if they are not used. This makes it hard to determine proper structure size.
				//     - If struct is within a Uniform buffer block, then it is possible because the element won't be optimized out of the buffer
				//     - If the struct is within a global buffer, it is impossible to determine actual size, since the element will be optimized out of the buffer too
				//     - Same issue happens with arrays, as OpenGL will optimize out array elements. With global buffers this makes it impossible to determine
				//       actual array size (e.g. suppose OpenGL optimized out few last elements)
				//        - Normal arrays work fine as OpenGL has utilities for reporting their actual size, but those do not work with structs

				CM_EXCEPT(NotImplementedException, "Structs are not supported.")
			}

			// GLSL will optimize out unused array indexes, so there's no guarantee that 0 is the first,
			// so we store the first one here
			int firstArrayIndex = 0;
			if(isInArray)
			{
				String& nameToSearch = cleanParamName;
				if(inStruct)
					nameToSearch = structName;

				auto arrayIndexFind = foundFirstArrayIndex.find(nameToSearch);
				if(arrayIndexFind == foundFirstArrayIndex.end())
				{
					foundFirstArrayIndex[nameToSearch] = arrayIdx;
				}

				firstArrayIndex = foundFirstArrayIndex[nameToSearch];
			}


			GLint uniformType;
			glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_TYPE, &uniformType);

			bool isSampler = false;
			switch(uniformType)
			{
			case GL_SAMPLER_1D:
			case GL_SAMPLER_2D:
			case GL_SAMPLER_3D:
			case GL_SAMPLER_CUBE:
				isSampler = true;
			}

			if(isSampler)
			{
				GpuParamObjectDesc samplerParam;
				samplerParam.name = paramName;
				samplerParam.slot = glGetUniformLocation(glProgram, uniformName);

				GpuParamObjectDesc textureParam;
				textureParam.name = paramName;
				textureParam.slot = samplerParam.slot;

				switch(uniformType)
				{
				case GL_SAMPLER_1D:
					samplerParam.type = GPOT_SAMPLER1D;
					textureParam.type = GPOT_TEXTURE1D;
					break;
				case GL_SAMPLER_2D:
					samplerParam.type = GPOT_SAMPLER2D;
					textureParam.type = GPOT_TEXTURE2D;
					break;
				case GL_SAMPLER_3D:
					samplerParam.type = GPOT_SAMPLER3D;
					textureParam.type = GPOT_TEXTURE3D;
					break;
				case GL_SAMPLER_CUBE:
					samplerParam.type = GPOT_SAMPLERCUBE;
					textureParam.type = GPOT_TEXTURECUBE;
					break;
				}

				returnParamDesc.samplers.insert(std::make_pair(paramName, samplerParam));
				returnParamDesc.textures.insert(std::make_pair(paramName, textureParam));
			}
			else
			{
				// If array index is larger than 0 and uniform is not a part of a struct,
				// it means we already processed it (struct arrays are processed differently)
				if(!inStruct && arrayIdx != 0)
					continue;

				GLint blockIndex;
				glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_BLOCK_INDEX, &blockIndex);

				GpuParamDataDesc gpuParam;

				if(isInArray)
					gpuParam.name = cleanParamName;
				else
					gpuParam.name = paramName;

				determineParamInfo(gpuParam, paramName, glProgram, index);

				if(blockIndex != -1)
				{
					GLint blockOffset;
					glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_OFFSET, &blockOffset);
					blockOffset = blockOffset / 4;

					gpuParam.gpuMemOffset = blockOffset;

					gpuParam.paramBlockSlot = blockIndex + 1; // 0 is reserved for globals

					String& blockName = blockSlotToName[gpuParam.paramBlockSlot];
					GpuParamBlockDesc& curBlockDesc = returnParamDesc.paramBlocks[blockName];

					gpuParam.cpuMemOffset = blockOffset;
					curBlockDesc.blockSize = std::max(curBlockDesc.blockSize, gpuParam.cpuMemOffset + gpuParam.arrayElementStride * gpuParam.arraySize);
				}
				else
				{
					gpuParam.gpuMemOffset = glGetUniformLocation(glProgram, uniformName);
					gpuParam.paramBlockSlot = 0;
					gpuParam.cpuMemOffset = globalBlockDesc.blockSize;

					globalBlockDesc.blockSize = std::max(globalBlockDesc.blockSize, gpuParam.cpuMemOffset + gpuParam.arrayElementStride * gpuParam.arraySize);
				}

				// If parameter is not a part of a struct we're done
				if(!inStruct)
				{
					returnParamDesc.params.insert(std::make_pair(gpuParam.name, gpuParam));
					continue;
				}

				// If the parameter is part of a struct, then we need to update the struct definition
				auto findExistingStruct = foundStructs.find(structName);

				// Create new definition if one doesn't exist
				if(findExistingStruct == foundStructs.end())
				{
					foundStructs[structName] = GpuParamDataDesc();
					GpuParamDataDesc& structDesc = foundStructs[structName];
					structDesc.type = GPDT_STRUCT;
					structDesc.name = structName;
					structDesc.arraySize = 1;
					structDesc.elementSize = 0;
					structDesc.arrayElementStride = 0;
					structDesc.gpuMemOffset = gpuParam.gpuMemOffset;
					structDesc.cpuMemOffset = gpuParam.cpuMemOffset;
					structDesc.paramBlockSlot = gpuParam.paramBlockSlot;
				}

				// Update struct with size of the new parameter
				GpuParamDataDesc& structDesc = foundStructs[structName];
				
				assert(gpuParam.cpuMemOffset >= structDesc.cpuMemOffset);
				if(arrayIdx == firstArrayIndex) // Determine element size only using the first array element
				{
					structDesc.elementSize = std::max(structDesc.elementSize, (gpuParam.cpuMemOffset - structDesc.cpuMemOffset) + gpuParam.arrayElementStride * gpuParam.arraySize);
					structDesc.arrayElementStride = structDesc.elementSize;
				}

				// New array element reached, determine arrayElementStride
				if(arrayIdx != firstArrayIndex)
				{
					UINT32 numElements = arrayIdx - firstArrayIndex;
					structDesc.arrayElementStride = (gpuParam.cpuMemOffset - structDesc.cpuMemOffset) / numElements; 
				}

				structDesc.arraySize = std::max(structDesc.arraySize, arrayIdx + 1);
			}
		}

		for(auto iter = foundStructs.begin(); iter != foundStructs.end(); ++iter)
			returnParamDesc.params.insert(std::make_pair(iter->first, iter->second));

		// Param blocks alway needs to be a multiple of 4, so make it so
		for(auto iter = returnParamDesc.paramBlocks.begin(); iter != returnParamDesc.paramBlocks.end(); ++iter)
		{
			GpuParamBlockDesc& blockDesc = iter->second;

			if(blockDesc.blockSize % 4 != 0)
				blockDesc.blockSize += (4 - (blockDesc.blockSize % 4));
		}

#if CM_DEBUG_MODE
		// Check if manually calculated and OpenGL buffer sizes match
		for(auto iter = returnParamDesc.paramBlocks.begin(); iter != returnParamDesc.paramBlocks.end(); ++iter)
		{
			if(iter->second.slot == 0)
				continue;

			GLint blockSize = 0;
			glGetActiveUniformBlockiv(glProgram, iter->second.slot - 1, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

			assert (blockSize % 4 == 0);
			blockSize = blockSize / 4;

			if(iter->second.blockSize != blockSize)
				CM_EXCEPT(InternalErrorException, "OpenGL specified and manual uniform block buffer sizes don't match!");
		}
#endif

		cm_free<ScratchAlloc>(uniformName);
	}

	void GLSLParamParser::determineParamInfo(GpuParamDataDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex)
	{
		GLint arraySize;
		glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_SIZE, &arraySize);
		desc.arraySize = arraySize;

		GLint uniformType;
		glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_TYPE, &uniformType);

		switch (uniformType)
		{
		case GL_BOOL:
			desc.type = GPDT_BOOL;
			desc.elementSize = 1;
			break;
		case GL_FLOAT:
			desc.type = GPDT_FLOAT1;
			desc.elementSize = 1;
			break;
		case GL_FLOAT_VEC2:
			desc.type = GPDT_FLOAT2;
			desc.elementSize = 2;
			break;
		case GL_FLOAT_VEC3:
			desc.type = GPDT_FLOAT3;
			desc.elementSize = 3;
			break;
		case GL_FLOAT_VEC4:
			desc.type = GPDT_FLOAT4;
			desc.elementSize = 4;
			break;
		case GL_INT:
			desc.type = GPDT_INT1;
			desc.elementSize = 1;
			break;
		case GL_INT_VEC2:
			desc.type = GPDT_INT2;
			desc.elementSize = 2;
			break;
		case GL_INT_VEC3:
			desc.type = GPDT_INT3;
			desc.elementSize = 3;
			break;
		case GL_INT_VEC4:
			desc.type = GPDT_INT4;
			desc.elementSize = 4;
			break;
		case GL_FLOAT_MAT2:
			desc.type = GPDT_MATRIX_2X2;
			desc.elementSize = 4;
			break;
		case GL_FLOAT_MAT3:
			desc.type = GPDT_MATRIX_3X3;
			desc.elementSize = 9;
			break;
		case GL_FLOAT_MAT4:
			desc.type = GPDT_MATRIX_4X4;
			desc.elementSize = 16;
			break;
		case GL_FLOAT_MAT2x3:
			desc.type = GPDT_MATRIX_2X3;
			desc.elementSize = 6;
			break;
		case GL_FLOAT_MAT3x2:
			desc.type = GPDT_MATRIX_3X2;
			desc.elementSize = 6;
			break;
		case GL_FLOAT_MAT2x4:
			desc.type = GPDT_MATRIX_2X4;
			desc.elementSize = 8;
			break;
		case GL_FLOAT_MAT4x2:
			desc.type = GPDT_MATRIX_4X2;
			desc.elementSize = 8;
			break;
		case GL_FLOAT_MAT3x4:
			desc.type = GPDT_MATRIX_3X4;
			desc.elementSize = 12;
			break;
		case GL_FLOAT_MAT4x3:
			desc.type = GPDT_MATRIX_4X3;
			desc.elementSize = 12;
			break;
		default:
			CM_EXCEPT(InternalErrorException, "Invalid shader parameter type: " + toString(uniformType) + " for parameter " + paramName);
		}

		if(arraySize > 1)
		{
			GLint arrayStride;
			glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_ARRAY_STRIDE, &arrayStride);

			if(arrayStride > 0)
			{
				assert (arrayStride % 4 == 0);

				desc.arrayElementStride = arrayStride / 4;
			}
			else
				desc.arrayElementStride = desc.elementSize;
		}
		else
			desc.arrayElementStride = desc.elementSize;
	}
}