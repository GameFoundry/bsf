#include "CmGLGpuParamBlock.h"

namespace CamelotEngine
{
	GLGpuParamBlock::GLGpuParamBlock(const GpuParamBlockDesc& desc)
		:GpuParamBlock(desc), mBufferInitialized(false), mGLHandle(0)
	{

	}

	GLGpuParamBlock::~GLGpuParamBlock()
	{

	}

	void GLGpuParamBlock::updateIfDirty()
	{
		if(mDirty)
		{
			if(!mBufferInitialized)
			{
				glGenBuffers(1, &mGLHandle);
				glBindBuffer(GL_UNIFORM_BUFFER, mGLHandle);
				glBufferData(mGLHandle, mSize, mData, GL_WRITE_ONLY);
			}
			else
				glBufferSubData(GL_UNIFORM_BUFFER, 0 , mSize, mData);
		}

		GpuParamBlock::updateIfDirty();
	}

	GpuParamBlockPtr GLGpuParamBlock::clone() const
	{
		std::shared_ptr<GLGpuParamBlock> clonedParamBlock(new GLGpuParamBlock(*this));
		clonedParamBlock->mData = new UINT8[mSize];
		memcpy(clonedParamBlock->mData, mData, mSize);

		return clonedParamBlock;
	}
}