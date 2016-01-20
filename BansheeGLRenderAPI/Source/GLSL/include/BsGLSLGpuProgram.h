//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuProgram.h"

namespace BansheeEngine 
{
	/**
	 * @brief	GPU program compiled from GLSL and usable by OpenGL.
	 */
    class BS_RSGL_EXPORT GLSLGpuProgramCore : public GpuProgramCore
    {
	public:
		~GLSLGpuProgramCore();

		/**
		 * @copydoc	GpuProgramCore::isSupported
		 */
		bool isSupported() const override;

		/**
		 * @brief	Gets internal OpenGL handle to the program.
		 */
		GLuint getGLHandle() const { return mGLHandle; }

		/**
		 * @brief	Gets an unique index for this GPU program. Each created GPU program is
		 *			assigned a unique index on creation.
		 */
		UINT32 getProgramID() const { return mProgramID; }

	private:
		friend class GLSLProgramFactory;

		GLSLGpuProgramCore(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, bool isAdjacencyInfoRequired);

		/**
		 * @copydoc GpuProgramCore::initialize
		 */
		void initialize() override;

	private:
		UINT32 mProgramID;
		GLuint mGLHandle;

		static UINT32 mVertexShaderCount;
		static UINT32 mFragmentShaderCount;
		static UINT32 mGeometryShaderCount;
		static UINT32 mHullShaderCount;
		static UINT32 mDomainShaderCount;
    };
}
