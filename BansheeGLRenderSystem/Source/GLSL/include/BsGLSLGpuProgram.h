#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuProgram.h"

namespace BansheeEngine 
{
	/**
	 * @brief	GPU program compiled from GLSL and usable by OpenGL.
	 */
    class BS_RSGL_EXPORT GLSLGpuProgram : public GpuProgram
    {
	public:
		~GLSLGpuProgram();

		/**
		 * @copydoc	GpuProgram::isSupported
		 */
		bool isSupported() const;

		/**
		 * @copydoc	GpuProgram::getLanguage
		 */
		const String& getLanguage() const;

		/**
		 * @brief	Gets vertex declaration that determines which input attributes does the GPU
		 *			program expect (and which attributes will it retrieve from the bound vertex buffer). 
		 *			Only valid for vertex programs.
		 */
		const VertexDeclaration& getInputAttributes() const { return *mVertexDeclaration; }

		/**
		 * @brief	Gets internal OpenGL handle to the program.
		 */
		const GLuint getGLHandle() const { return mGLHandle; }

		/**
		 * @brief	Gets an unique index for this GPU program. Each created GPU program is
		 *			assigned a unique index on creation.
		 */
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

		VertexDeclarationPtr mVertexDeclaration;
		
		static UINT32 mVertexShaderCount;
		static UINT32 mFragmentShaderCount;
		static UINT32 mGeometryShaderCount;
		static UINT32 mHullShaderCount;
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
