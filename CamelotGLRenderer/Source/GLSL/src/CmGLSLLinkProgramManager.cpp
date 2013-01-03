/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/

#include "CmGLSLLinkProgramManager.h"
#include "CmGLSLGpuProgram.h"
#include "CmGLSLProgram.h"
#include "CmGpuParamDesc.h"

namespace CamelotEngine 
{
	template <class T>
	inline void hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	}

	::std::size_t GLSLLinkProgramManager::LinkProgramKeyHashFunction::operator()(const GLSLLinkProgramManager::LinkProgramKey &key) const
	{
		std::size_t seed = 0;
		hash_combine(seed, key.vertexProgKey);
		hash_combine(seed, key.fragmentProgKey);
		hash_combine(seed, key.geometryProgKey);
		hash_combine(seed, key.hullProgKey);
		hash_combine(seed, key.domainProgKey);

		return seed;
	}

	bool GLSLLinkProgramManager::LinkProgramKeyEqual::operator()(const GLSLLinkProgramManager::LinkProgramKey &a, const GLSLLinkProgramManager::LinkProgramKey &b) const
	{
		return a.vertexProgKey == b.vertexProgKey && a.fragmentProgKey == b.fragmentProgKey && a.geometryProgKey == b.geometryProgKey &&
			a.hullProgKey == b.hullProgKey && a.domainProgKey == b.domainProgKey;
	}

	//-----------------------------------------------------------------------
	GLSLLinkProgramManager::GLSLLinkProgramManager(void) : mActiveVertexGpuProgram(nullptr),
		mActiveGeometryGpuProgram(nullptr), mActiveFragmentGpuProgram(nullptr), mActiveHullGpuProgram(nullptr),
		mActiveDomainGpuProgram(nullptr), mActiveLinkProgram(nullptr)
	{

	}

	//-----------------------------------------------------------------------
	GLSLLinkProgramManager::~GLSLLinkProgramManager(void)
	{
		// iterate through map container and delete link programs
		for (LinkProgramIterator currentProgram = mLinkPrograms.begin();
			currentProgram != mLinkPrograms.end(); ++currentProgram)
		{
			delete currentProgram->second;
		}
	}

	//-----------------------------------------------------------------------
	GLSLLinkProgram* GLSLLinkProgramManager::getActiveLinkProgram(void)
	{
		// if there is an active link program then return it
		if (mActiveLinkProgram)
			return mActiveLinkProgram;

		LinkProgramKey key;
		key.vertexProgKey = mActiveVertexGpuProgram != nullptr ? mActiveVertexGpuProgram->getProgramID() : 0;
		key.fragmentProgKey = mActiveFragmentGpuProgram != nullptr ? mActiveFragmentGpuProgram->getProgramID() : 0;
		key.geometryProgKey = mActiveGeometryGpuProgram != nullptr ? mActiveGeometryGpuProgram->getProgramID() : 0;
		key.hullProgKey = mActiveHullGpuProgram != nullptr ? mActiveHullGpuProgram->getProgramID() : 0;
		key.domainProgKey = mActiveDomainGpuProgram != nullptr ? mActiveDomainGpuProgram->getProgramID() : 0;

		// find the key in the hash map
		LinkProgramIterator programFound = mLinkPrograms.find(key);
		// program object not found for key so need to create it
		if (programFound == mLinkPrograms.end())
		{
			mActiveLinkProgram = new GLSLLinkProgram(mActiveVertexGpuProgram, mActiveGeometryGpuProgram, mActiveFragmentGpuProgram, mActiveHullGpuProgram, mActiveDomainGpuProgram);
			mLinkPrograms[key] = mActiveLinkProgram;
		}
		else
		{
			// found a link program in map container so make it active
			mActiveLinkProgram = programFound->second;
		}

		// make the program object active
		if (mActiveLinkProgram) 
			mActiveLinkProgram->activate();

		return mActiveLinkProgram;
	}

	//-----------------------------------------------------------------------
	void GLSLLinkProgramManager::setActiveFragmentShader(GLSLGpuProgram* fragmentGpuProgram)
	{
		if (fragmentGpuProgram != mActiveFragmentGpuProgram)
		{
			mActiveFragmentGpuProgram = fragmentGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = nullptr;
			// change back to fixed pipeline
			glUseProgramObjectARB(0);
		}
	}

	//-----------------------------------------------------------------------
	void GLSLLinkProgramManager::setActiveVertexShader(GLSLGpuProgram* vertexGpuProgram)
	{
		if (vertexGpuProgram != mActiveVertexGpuProgram)
		{
			mActiveVertexGpuProgram = vertexGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = nullptr;
			// change back to fixed pipeline
			glUseProgramObjectARB(0);
		}
	}
	//-----------------------------------------------------------------------
	void GLSLLinkProgramManager::setActiveGeometryShader(GLSLGpuProgram* geometryGpuProgram)
	{
		if (geometryGpuProgram != mActiveGeometryGpuProgram)
		{
			mActiveGeometryGpuProgram = geometryGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = nullptr;
			// change back to fixed pipeline
			glUseProgramObjectARB(0);
		}
	}
	//-----------------------------------------------------------------------
	void GLSLLinkProgramManager::setActiveHullShader(GLSLGpuProgram* hullGpuProgram)
	{
		if (hullGpuProgram != mActiveHullGpuProgram)
		{
			mActiveHullGpuProgram = hullGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = nullptr;
			// change back to fixed pipeline
			glUseProgramObjectARB(0);
		}
	}
	//-----------------------------------------------------------------------
	void GLSLLinkProgramManager::setActiveDomainShader(GLSLGpuProgram* domainGpuProgram)
	{
		if (domainGpuProgram != mActiveDomainGpuProgram)
		{
			mActiveDomainGpuProgram = domainGpuProgram;
			// ActiveLinkProgram is no longer valid
			mActiveLinkProgram = nullptr;
			// change back to fixed pipeline
			glUseProgramObjectARB(0);
		}
	}
	//---------------------------------------------------------------------
	void GLSLLinkProgramManager::extractGpuParams(GLSLLinkProgram* linkProgram, GpuParamDesc& returnParamDesc)
	{
		assert(linkProgram != nullptr);

		// scan through the active uniforms and add them to the reference list
		GLint uniformCount = 0;
		GLuint glProgram = linkProgram->getGLHandle();

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
	//---------------------------------------------------------------------
	void GLSLLinkProgramManager::determineParamInfo(GpuParamMemberDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex)
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
