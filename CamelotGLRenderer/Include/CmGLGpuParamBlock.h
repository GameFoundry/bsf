#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuParamBlock.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLGpuParamBlockBuffer : public GpuParamBlockBuffer
	{
	public:
		GLGpuParamBlockBuffer(UINT32 size, GpuParamBlockUsage usage);
		~GLGpuParamBlockBuffer();

		/**
		 * @copydoc CpuParamBlockBuffer::writeAll.
		 */
		void writeAll(const void* data);

		GLuint getGLHandle() const { return mGLHandle; }

	private:
		GLuint mGLHandle;
	};

	class CM_RSGL_EXPORT GLGpuParamBlock : public GpuParamBlock
	{
	protected:
		/**
		 * @copydoc GpuParamBlock::createBuffer.
		 */
		GpuParamBlockBuffer* createBuffer() const;
	};
}