#pragma once

#include "CmGLPrerequisites.h"
#include "CmVertexDeclaration.h"
#include "CmDebug.h"
#include "CmException.h"
#include "CmGpuParamDesc.h"

namespace CamelotEngine
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
				return parseUnsignedInt(indexStr, -1);
			}
		}

		return -1;
	}

	class GLSLParamParser
	{
	public:
		void buildUniformDescriptions(GLuint glProgram, GpuParamDesc& returnParamDesc);
		void buildVertexDeclaration(GLuint glProgram, VertexDeclaration& declaration);

	private:
		void determineParamInfo(GpuParamMemberDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex);

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

		GLchar* attributeName = new GLchar[maxNameSize];
		UINT32 offset = 0;

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

				declaration.addElement(index, offset, type, semantic, index);
				offset += attribSize;
			}
			else
			{
				LOGWRN("Cannot determine vertex input attribute type for attribute: " + String(attributeName));
			}
		}

		delete[] attributeName;
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
			GLSLAttribute("position", VES_POSITION),
			GLSLAttribute("normal", VES_NORMAL),
			GLSLAttribute("tangent", VES_TANGENT),
			GLSLAttribute("bitangent", VES_BITANGENT),
			GLSLAttribute("texcoord", VES_TEXCOORD),
			GLSLAttribute("color", VES_COLOR),
			GLSLAttribute("blendweights", VES_BLEND_WEIGHTS),
			GLSLAttribute("blendindices", VES_BLEND_INDICES)
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

		GLchar* uniformName = new GLchar[maxBufferSize];

		GpuParamBlockDesc globalBlockDesc;
		globalBlockDesc.slot = 0;
		globalBlockDesc.name = "CM_INTERNAL_Globals";
		globalBlockDesc.blockSize = 0;

		UINT32 textureSlot = 0;

		returnParamDesc.paramBlocks[globalBlockDesc.name] = globalBlockDesc;

		GLint uniformBlockCount = 0;
		glGetProgramiv(glProgram, GL_ACTIVE_UNIFORM_BLOCKS, &uniformBlockCount);

		map<UINT32, String>::type blockSlotToName;
		for (GLuint index = 0; index < (GLuint)uniformBlockCount; index++)
		{
			GLsizei unusedSize = 0;
			glGetActiveUniformBlockName(glProgram, index, maxBufferSize, &unusedSize, uniformName);

			GpuParamBlockDesc newBlockDesc;
			newBlockDesc.slot = index + 1;
			newBlockDesc.name = uniformName;
			newBlockDesc.blockSize = 0;

			returnParamDesc.paramBlocks[newBlockDesc.name] = newBlockDesc;
			blockSlotToName.insert(std::make_pair(newBlockDesc.slot, newBlockDesc.name));
		}

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

			// If the uniform name has a "[" in it then its an array element uniform.
			String::size_type arrayStart = paramName.find("[");
			if (arrayStart != String::npos)
			{
				// if not the first array element then skip it and continue to the next uniform
				if (paramName.compare(arrayStart, paramName.size() - 1, "[0]") != 0)
					CM_EXCEPT(NotImplementedException, "No support for array parameters yet.");

				paramName = paramName.substr(0, arrayStart);
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
				GpuParamSpecialDesc samplerParam;
				samplerParam.name = paramName;
				samplerParam.slot = glGetUniformLocation(glProgram, uniformName);

				GpuParamSpecialDesc textureParam;
				textureParam.name = paramName;
				textureParam.slot = samplerParam.slot;

				switch(uniformType)
				{
				case GL_SAMPLER_1D:
					samplerParam.type = GST_SAMPLER1D;
					textureParam.type = GST_TEXTURE1D;
					break;
				case GL_SAMPLER_2D:
					samplerParam.type = GST_SAMPLER2D;
					textureParam.type = GST_TEXTURE2D;
					break;
				case GL_SAMPLER_3D:
					samplerParam.type = GST_SAMPLER3D;
					textureParam.type = GST_TEXTURE3D;
					break;
				case GL_SAMPLER_CUBE:
					samplerParam.type = GST_SAMPLERCUBE;
					textureParam.type = GST_TEXTURECUBE;
					break;
				}

				returnParamDesc.samplers.insert(std::make_pair(paramName, samplerParam));
				returnParamDesc.textures.insert(std::make_pair(paramName, textureParam));
			}
			else
			{
				GpuParamMemberDesc gpuParam;
				gpuParam.name = paramName;

				GLint blockIndex;
				glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_BLOCK_INDEX, &blockIndex);

				determineParamInfo(gpuParam, paramName, glProgram, index);

				if(blockIndex != -1)
				{
					GLint blockOffset;
					glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_OFFSET, &blockOffset);
					gpuParam.gpuMemOffset = blockOffset;

					GLint arrayStride;
					glGetActiveUniformsiv(glProgram, 1, &index, GL_UNIFORM_ARRAY_STRIDE, &arrayStride);
					gpuParam.elementSize = arrayStride;

					gpuParam.paramBlockSlot = blockIndex + 1; // 0 is reserved for globals

					String& blockName = blockSlotToName[gpuParam.paramBlockSlot];
					GpuParamBlockDesc& curBlockDesc = returnParamDesc.paramBlocks[blockName];

					gpuParam.cpuMemOffset = curBlockDesc.blockSize;
					curBlockDesc.blockSize += gpuParam.elementSize * gpuParam.arraySize;
				}
				else
				{
					gpuParam.gpuMemOffset = glGetUniformLocationARB(glProgram, uniformName);
					gpuParam.paramBlockSlot = 0;
					gpuParam.cpuMemOffset = globalBlockDesc.blockSize;

					globalBlockDesc.blockSize += gpuParam.elementSize * gpuParam.arraySize;
				}

				returnParamDesc.params.insert(std::make_pair(gpuParam.name, gpuParam));
			}
		}

		delete[] uniformName;
	}

	void GLSLParamParser::determineParamInfo(GpuParamMemberDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex)
	{
		GLint arraySize;
		glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_SIZE, &arraySize);
		desc.arraySize = arraySize;

		GLint uniformType;
		glGetActiveUniformsiv(programHandle, 1, &uniformIndex, GL_UNIFORM_TYPE, &uniformType);

		switch (uniformType)
		{
		case GL_BOOL:
			desc.type = GMT_BOOL;
			desc.elementSize = 1;
			break;
		case GL_FLOAT:
			desc.type = GMT_FLOAT1;
			desc.elementSize = 1;
			break;
		case GL_FLOAT_VEC2:
			desc.type = GMT_FLOAT2;
			desc.elementSize = 2;
			break;
		case GL_FLOAT_VEC3:
			desc.type = GMT_FLOAT3;
			desc.elementSize = 3;
			break;
		case GL_FLOAT_VEC4:
			desc.type = GMT_FLOAT4;
			desc.elementSize = 4;
			break;
		case GL_INT:
			desc.type = GMT_INT1;
			desc.elementSize = 1;
			break;
		case GL_INT_VEC2:
			desc.type = GMT_INT2;
			desc.elementSize = 2;
			break;
		case GL_INT_VEC3:
			desc.type = GMT_INT3;
			desc.elementSize = 3;
			break;
		case GL_INT_VEC4:
			desc.type = GMT_INT4;
			desc.elementSize = 4;
			break;
		case GL_FLOAT_MAT2:
			desc.type = GMT_MATRIX_2X2;
			desc.elementSize = 4;
			break;
		case GL_FLOAT_MAT3:
			desc.type = GMT_MATRIX_3X3;
			desc.elementSize = 9;
			break;
		case GL_FLOAT_MAT4:
			desc.type = GMT_MATRIX_4X4;
			desc.elementSize = 16;
			break;
		case GL_FLOAT_MAT2x3:
			desc.type = GMT_MATRIX_2X3;
			desc.elementSize = 6;
			break;
		case GL_FLOAT_MAT3x2:
			desc.type = GMT_MATRIX_3X2;
			desc.elementSize = 6;
			break;
		case GL_FLOAT_MAT2x4:
			desc.type = GMT_MATRIX_2X4;
			desc.elementSize = 8;
			break;
		case GL_FLOAT_MAT4x2:
			desc.type = GMT_MATRIX_4X2;
			desc.elementSize = 8;
			break;
		case GL_FLOAT_MAT3x4:
			desc.type = GMT_MATRIX_3X4;
			desc.elementSize = 12;
			break;
		case GL_FLOAT_MAT4x3:
			desc.type = GMT_MATRIX_4X3;
			desc.elementSize = 12;
			break;
		default:
			CM_EXCEPT(InternalErrorException, "Invalid shader parameter type: " + toString(uniformType) + " for parameter " + paramName);
		}
	}
}