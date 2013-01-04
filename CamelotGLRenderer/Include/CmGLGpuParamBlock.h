#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuParamBlock.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLGpuParamBlock : public GpuParamBlock
	{
	public:
		GLGpuParamBlock(const GpuParamBlockDesc& desc);
		~GLGpuParamBlock();

		virtual void updateIfDirty();
		virtual GpuParamBlockPtr clone() const;

		GLuint getGLHandle() const { return mGLHandle; }

	private:
		GLuint mGLHandle;
		bool mBufferInitialized;
	};
}