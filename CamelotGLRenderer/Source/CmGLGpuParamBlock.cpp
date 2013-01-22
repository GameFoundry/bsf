#include "CmGLGpuParamBlock.h"

namespace CamelotEngine
{
	GLGpuParamBlock::GLGpuParamBlock(const GpuParamBlockDesc& desc, GpuParamBlockUsage usage)
		:GpuParamBlock(desc, usage), mGLSharedData(nullptr)
	{
		mGLSharedData = new GLGpuParamBlockSharedData();
	}

	GLGpuParamBlock::~GLGpuParamBlock()
	{
		if(mOwnsSharedData)
		{
			glDeleteBuffers(1, &mGLSharedData->mGLHandle);
			delete mGLSharedData;
		}
	}

	void GLGpuParamBlock::updateIfDirty()
	{
		if(!sharedData->mInitialized)
		{
			glGenBuffers(1, &mGLSharedData->mGLHandle);
			glBindBuffer(GL_UNIFORM_BUFFER, mGLSharedData->mGLHandle);
			glBufferData(GL_UNIFORM_BUFFER, mSize, (GLvoid*)mData, GL_DYNAMIC_DRAW);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);

			sharedData->mInitialized = true;
		}

		if(sharedData->mDirty)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, mGLSharedData->mGLHandle);
			glBufferSubData(GL_UNIFORM_BUFFER, 0 , mSize, mData);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		GpuParamBlock::updateIfDirty();
	}

	GpuParamBlockPtr GLGpuParamBlock::clone() const
	{
		std::shared_ptr<GLGpuParamBlock> clonedParamBlock(new GLGpuParamBlock(*this));
		clonedParamBlock->mData = new UINT8[mSize];
		clonedParamBlock->mOwnsSharedData = false;
		clonedParamBlock->mGLSharedData = mGLSharedData;
		memcpy(clonedParamBlock->mData, mData, mSize);

		return clonedParamBlock;
	}
}