#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuParamBlockBuffer.h"

namespace BansheeEngine
{
	/**
	 * @brief	OpenGL implementation of a GPU parameter buffer (Uniform buffer).
	 */
	class BS_RSGL_EXPORT GLGpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		GLGpuParamBlockBuffer();

		/**
		 * @copydoc GpuParamBlockBuffer::writeAll.
		 */
		void writeData(const UINT8* data);

		/**
		 * @copydoc GpuParamBlockBuffer::readAll.
		 */
		void readData(UINT8* data) const;

		/**
		 * @brief	Returns internal OpenGL uniform buffer handle.
		 */
		GLuint getGLHandle() const { return mGLHandle; }
	protected:
		/**
		 * @copydoc CoreGpuObject::initialize_internal.
		 */
		virtual void initialize_internal();

		/**
		 * @copydoc CoreGpuObject::destroy_internal.
		 */
		virtual void destroy_internal();

	private:
		GLuint mGLHandle;
	};
}