#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuParamBlockBuffer.h"
#include "CmCommonEnums.h"

namespace BansheeEngine
{
	class CM_RSGL_EXPORT GLGpuParamBlockBuffer : public GpuParamBlockBuffer
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