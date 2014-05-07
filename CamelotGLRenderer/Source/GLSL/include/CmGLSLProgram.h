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
#ifndef __GLSLProgram_H__
#define __GLSLProgram_H__

#include "CmGLPrerequisites.h"
#include "CmHighLevelGpuProgram.h"

namespace BansheeEngine {
    /** Specialisation of HighLevelGpuProgram to provide support for OpenGL 
        Shader Language (GLSL).
    @remarks
		GLSL has no target assembler or entry point specification like DirectX 9 HLSL.
		Vertex and Fragment shaders only have one entry point called "main".  
		When a shader is compiled, microcode is generated but can not be accessed by
		the application.
		GLSL also does not provide assembler low level output after compiling.  The GL Render
		system assumes that the Gpu program is a GL Gpu program so GLSLProgram will create a 
		GLSLGpuProgram that is subclassed from GLGpuProgram for the low level implementation.
		The GLSLProgram class will create a shader object and compile the source but will
		not create a program object.  It's up to GLSLGpuProgram class to request a program object
		to link the shader object to.

	@note
		GLSL supports multiple modular shader objects that can be attached to one program
		object to form a single shader.  This is supported through the "attach" material script
		command.  All the modules to be attached are listed on the same line as the attach command
		seperated by white space.
        
    */
    class CM_RSGL_EXPORT GLSLProgram : public HighLevelGpuProgram
    {
    public:
		~GLSLProgram();

		const GLuint getGLHandle() const { return mGLHandle; }

		/** Sets the preprocessor defines use to compile the program. */
		void setPreprocessorDefines(const String& defines) { mPreprocessorDefines = defines; }
		/** Sets the preprocessor defines use to compile the program. */
		const String& getPreprocessorDefines(void) const { return mPreprocessorDefines; }

        /// Overridden from GpuProgram
        const String& getLanguage(void) const;

		/** Returns the operation type that this geometry program expects to
			receive as input
		*/
		virtual DrawOperationType getInputOperationType(void) const 
		{ return mInputOperationType; }
		/** Returns the operation type that this geometry program will emit
		*/
		virtual DrawOperationType getOutputOperationType(void) const 
		{ return mOutputOperationType; }
		/** Returns the maximum number of vertices that this geometry program can
			output in a single run
		*/
		virtual int getMaxOutputVertices(void) const { return mMaxOutputVertices; }

		/** Sets the operation type that this geometry program expects to receive
		*/
		virtual void setInputOperationType(DrawOperationType operationType) 
		{ mInputOperationType = operationType; }
		/** Set the operation type that this geometry program will emit
		*/
		virtual void setOutputOperationType(DrawOperationType operationType) 
		{ mOutputOperationType = operationType; }
		/** Set the maximum number of vertices that a single run of this geometry program
			can emit.
		*/
		virtual void setMaxOutputVertices(int maxOutputVertices) 
		{ mMaxOutputVertices = maxOutputVertices; }

		const VertexDeclaration& getInputAttributes() const { return *mVertexDeclaration; }

		bool isSupported() const;

	protected:
		friend class GLSLProgramFactory;

		GLSLProgram(const String& source, const String& entryPoint, 
			GpuProgramType gptype, GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, 
			bool isAdjacencyInfoRequired = false);

		/**
		 * @copydoc HighLevelGpuProgram::initialize_internal()
		 */
        void initialize_internal();

		/**
		 * @copydoc HighLevelGpuProgram::destroy_internal()
		 */
		void destroy_internal();

	private:
		/// GL handle for shader object
		GLuint mGLHandle;
		/// flag indicating if shader object successfully compiled
		GLint mCompiled;
		/// The input operation type for this (geometry) program
		DrawOperationType mInputOperationType;
		/// The output operation type for this (geometry) program
		DrawOperationType mOutputOperationType;
		/// The maximum amount of vertices that this (geometry) program can output
		int mMaxOutputVertices;
		/// Preprocessor options
		String mPreprocessorDefines;
		VertexDeclarationPtr mVertexDeclaration;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GLSLProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}

#endif // __GLSLProgram_H__
