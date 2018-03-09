//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsGLPrerequisites.h"
#include "RenderAPI/BsGpuParamBlockBuffer.h"

namespace bs { namespace ct
{
	/** @addtogroup GL
	 *  @{
	 */

	/**	OpenGL implementation of a GPU parameter buffer (Uniform buffer). */
	class GLGpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		GLGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage, GpuDeviceFlags deviceMask);
		~GLGpuParamBlockBuffer();

		/** @copydoc GpuParamBlockBuffer::writeToGPU */
		void writeToGPU(const UINT8* data, UINT32 queueIdx = 0) override;

		/**	Returns internal OpenGL uniform buffer handle. */
		GLuint getGLHandle() const { return mGLHandle; }
	protected:
		/** @copydoc GpuParamBlockBuffer::initialize */
		void initialize() override ;

	private:
		GLuint mGLHandle;
	};

	/** @} */
}}