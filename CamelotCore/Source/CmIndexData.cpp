#include "CmIndexData.h"
#include "CmHardwareBufferManager.h"
#include "CmVertexBuffer.h"
#include "CmIndexBuffer.h"
#include "CmVector3.h"
#include "CmAABox.h"
#include "CmException.h"
#include "CmRenderSystem.h"

namespace BansheeEngine 
{
	IndexData::IndexData()
		:indexCount(0), indexStart(0)
	{ }

	IndexData::~IndexData()
	{ }
}
