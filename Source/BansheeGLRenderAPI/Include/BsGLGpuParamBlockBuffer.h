//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	OpenGL implementation of a GPU parameter buffer (Uniform buffer). */
	class BS_RSGL_EXPORT GLGpuParamBlockBufferCore : public GpuParamBlockBufferCore
	{
	public:
		GLGpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage);
		~GLGpuParamBlockBufferCore();

		/** @copydoc GpuParamBlockBufferCore::writeData */
		void writeToGPU(const UINT8* data) override;

		/** @copydoc GpuParamBlockBufferCore::readData */
		void readFromGPU(UINT8* data) const override;

		/**	Returns internal OpenGL uniform buffer handle. */
		GLuint getGLHandle() const { return mGLHandle; }
	protected:
		/** @copydoc GpuParamBlockBufferCore::initialize */
		virtual void initialize()override ;

	private:
		GLuint mGLHandle;
	};

	/** @} */
}