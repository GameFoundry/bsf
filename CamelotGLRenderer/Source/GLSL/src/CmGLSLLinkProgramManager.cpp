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
}
