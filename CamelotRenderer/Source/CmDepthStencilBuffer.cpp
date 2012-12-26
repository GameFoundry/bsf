#include "CmDepthStencilBuffer.h"
#include "CmRenderTarget.h"

namespace CamelotEngine
{
	DepthStencilBuffer::DepthStencilBuffer(UINT32 bitDepth, UINT32 width, UINT32 height, UINT32 fsaa, const String &fsaaHint)
		: mBitDepth(bitDepth)
		, mWidth(width)
		, mHeight(height)
		, mFsaa(fsaa)
		, mFsaaHint(fsaaHint)
	{

	}

	DepthStencilBuffer::~DepthStencilBuffer()
	{

	}

	bool DepthStencilBuffer::isCompatible(RenderTarget *renderTarget) const
	{
		if( this->getWidth() >= renderTarget->getWidth() &&
			this->getHeight() >= renderTarget->getHeight() &&
			this->getFsaa() == renderTarget->getFSAA() )
		{
			return true;
		}

		return false;
	}
}