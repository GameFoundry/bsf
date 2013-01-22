#pragma once

#include "CmGLPrerequisites.h"
#include "CmGpuParamBlock.h"
#include "CmCommonEnums.h"

namespace CamelotEngine
{
	class CM_RSGL_EXPORT GLGpuParamBlock : public GpuParamBlock
	{
	private:
		struct GLGpuParamBlockSharedData
		{
			GLuint mGLHandle;
		};

	public:
		GLGpuParamBlock(const GpuParamBlockDesc& desc, GpuParamBlockUsage usage);
		~GLGpuParamBlock();

		virtual void updateIfDirty();
		virtual GpuParamBlockPtr clone() const;

		GLuint getGLHandle() const { return mGLSharedData->mGLHandle; }

	private:
		GLGpuParamBlockSharedData* mGLSharedData;
	};
}