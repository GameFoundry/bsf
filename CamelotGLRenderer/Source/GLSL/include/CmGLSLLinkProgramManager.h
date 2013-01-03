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
#ifndef __GLSLLinkProgramManager_H__
#define __GLSLLinkProgramManager_H__

#include "CmGLPrerequisites.h"
#include "CmModule.h"

#include "CmGLSLExtSupport.h"
#include "CmGLSLLinkProgram.h"

namespace CamelotEngine {


	/** Camelot assumes that there are seperate shader programs to deal with but
		GLSL has one program object that represents the active shader objects
		during a rendering state.  GLSL shader objects are compiled seperately and then attached to a 
		program object and then theprogram object is linked.  Since Ogre can only handle one vertex 
		program and one fragmentprogram being active in a pass, the GLSL Link Program Manager does the same.  
		The GLSL Link program manager acts as a state machine and activates a program object based on the active
		shader program.  
	*/

	class CM_RSGL_EXPORT GLSLLinkProgramManager : public Module<GLSLLinkProgramManager>
	{
	private:
		struct LinkProgramKey
		{
			UINT32 vertexProgKey;
			UINT32 fragmentProgKey;
			UINT32 geometryProgKey;
			UINT32 hullProgKey;
			UINT32 domainProgKey;
		};

		class LinkProgramKeyHashFunction 
		{
		public:
			::std::size_t operator()(const LinkProgramKey &key) const;
		};

		class LinkProgramKeyEqual 
		{
		public:
			bool operator()(const LinkProgramKey &a, const LinkProgramKey &b) const;
		};
	
		typedef std::unordered_map<LinkProgramKey, GLSLLinkProgram*, LinkProgramKeyHashFunction, LinkProgramKeyEqual> LinkProgramMap;
		typedef LinkProgramMap::iterator LinkProgramIterator;

		/// container holding previously created program objects 
		LinkProgramMap mLinkPrograms; 

		/// active objects defining the active rendering gpu state
		GLSLGpuProgram* mActiveVertexGpuProgram;
		GLSLGpuProgram* mActiveGeometryGpuProgram;
		GLSLGpuProgram* mActiveFragmentGpuProgram;
		GLSLGpuProgram* mActiveHullGpuProgram;
		GLSLGpuProgram* mActiveDomainGpuProgram;
		GLSLLinkProgram* mActiveLinkProgram;

		void determineParamInfo(GpuParamMemberDesc& desc, const String& paramName, GLuint programHandle, GLuint uniformIndex);

	public:

		GLSLLinkProgramManager(void);
		~GLSLLinkProgramManager(void);

		/**
			Get the program object that links the two active shader objects together
			if a program object was not already created and linked a new one is created and linked
		*/
		GLSLLinkProgram* getActiveLinkProgram(void);

		/** Set the active fragment shader for the next rendering state.
			The active program object will be cleared.
		*/
		void setActiveFragmentShader(GLSLGpuProgram* fragmentGpuProgram);

		/** Set the active geometry shader for the next rendering state.
			The active program object will be cleared.
		*/
		void setActiveGeometryShader(GLSLGpuProgram* geometryGpuProgram);

		/** Set the active vertex shader for the next rendering state.
			The active program object will be cleared.
		*/
		void setActiveVertexShader(GLSLGpuProgram* vertexGpuProgram);

		/** Set the active hull shader for the next rendering state.
			The active program object will be cleared.
		*/
		void setActiveHullShader(GLSLGpuProgram* hullGpuProgram);

		/** Set the active domain shader for the next rendering state.
			The active program object will be cleared.
		*/
		void setActiveDomainShader(GLSLGpuProgram* domainGpuProgram);

		void extractGpuParams(GLSLLinkProgram* linkProgram, GpuParamDesc& returnParamDesc);
	};

}

#endif // __GLSLLinkProgramManager_H__
