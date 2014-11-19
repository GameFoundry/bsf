#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	OpenGL implementation of a GPU parameter buffer (Uniform buffer).
	 */
	class BS_RSGL_EXPORT GLGpuParamBlockBufferCore : public GpuParamBlockBufferCore
	{
	public:
		GLGpuParamBlockBufferCore(UINT32 size, GpuParamBlockUsage usage);

		/**
		 * @copydoc GpuParamBlockBufferCore::writeData
		 */
		void writeToGPU(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBufferCore::readData
		 */
		void readFromGPU(UINT8* data) const;

		/**
		 * @brief	Returns internal OpenGL uniform buffer handle.
		 */
		GLuint getGLHandle() const { return mGLHandle; }
	protected:
		/**
		 * @copydoc GpuParamBlockBufferCore::initialize
		 */
		virtual void initialize();

		/**
		 * @copydoc GpuParamBlockBufferCore::destroy
		 */
		virtual void destroy();

	private:
		GLuint mGLHandle;
	};
}