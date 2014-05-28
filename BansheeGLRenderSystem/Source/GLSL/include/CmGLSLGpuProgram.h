#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuProgram.h"

namespace BansheeEngine {

    /** GLSL low level compiled shader object - this class is used to get at the linked program object 
		and provide an interface for GLRenderSystem calls.  GLSL does not provide access to the
		low level code of the shader so this class is really just a dummy place holder.
		GLSL uses a program object to represent the active vertex and fragment programs used
		but Ogre materials maintain seperate instances of the active vertex and fragment programs
		which creates a small problem for GLSL integration.  The GLSLGpuProgram class provides the
		interface between the GLSLLinkProgramManager , GLRenderSystem, and the active GLSLProgram
		instances.
	*/
    class BS_RSGL_EXPORT GLSLGpuProgram : public GpuProgram
    {
	public:
		~GLSLGpuProgram();

		bool isSupported() const;

		const GLuint getGLHandle() const { return mGLHandle; }

		/** Sets the preprocessor defines use to compile the program. */
		void setPreprocessorDefines(const String& defines) { mPreprocessorDefines = defines; }
		/** Sets the preprocessor defines use to compile the program. */
		const String& getPreprocessorDefines() const { return mPreprocessorDefines; }

		/// Overridden from GpuProgram
		const String& getLanguage() const;

		/** Returns the operation type that this geometry program expects to
		receive as input
		*/
		virtual DrawOperationType getInputOperationType() const
		{
			return mInputOperationType;
		}
		/** Returns the operation type that this geometry program will emit
		*/
		virtual DrawOperationType getOutputOperationType() const
		{
			return mOutputOperationType;
		}
		/** Returns the maximum number of vertices that this geometry program can
		output in a single run
		*/
		virtual int getMaxOutputVertices() const { return mMaxOutputVertices; }

		/** Sets the operation type that this geometry program expects to receive
		*/
		virtual void setInputOperationType(DrawOperationType operationType)
		{
			mInputOperationType = operationType;
		}
		/** Set the operation type that this geometry program will emit
		*/
		virtual void setOutputOperationType(DrawOperationType operationType)
		{
			mOutputOperationType = operationType;
		}
		/** Set the maximum number of vertices that a single run of this geometry program
		can emit.
		*/
		virtual void setMaxOutputVertices(int maxOutputVertices)
		{
			mMaxOutputVertices = maxOutputVertices;
		}

		const VertexDeclaration& getInputAttributes() const { return *mVertexDeclaration; }

		/// Get the assigned GL program id
		const UINT32 getProgramID() const { return mProgramID; }

	private:
		friend class GLSLProgramFactory;

		GLSLGpuProgram(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool isAdjacencyInfoRequired);

		/**
		* @copydoc GpuProgram::initialize_internal()
		*/
		void initialize_internal();

		/**
		* @copydoc GpuProgram::destroy_internal()
		*/
		void destroy_internal();

	private:
		UINT32 mProgramID;
		GLuint mGLHandle;

		DrawOperationType mInputOperationType;
		DrawOperationType mOutputOperationType;
		int mMaxOutputVertices;

		String mPreprocessorDefines;
		VertexDeclarationPtr mVertexDeclaration;
		
		/// keep track of the number of vertex shaders created
		static UINT32 mVertexShaderCount;
		/// keep track of the number of fragment shaders created
		static UINT32 mFragmentShaderCount;
		/// keep track of the number of geometry shaders created
		static UINT32 mGeometryShaderCount;
		/// keep track of the number of hull shaders created
		static UINT32 mHullShaderCount;
		/// keep track of the number of domain shaders created
		static UINT32 mDomainShaderCount;

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GLSLGpuProgramRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
    };
}
