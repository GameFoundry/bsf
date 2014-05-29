#include "BsIndexData.h"
#include "BsHardwareBufferManager.h"
#include "BsVertexBuffer.h"
#include "BsIndexBuffer.h"
#include "BsVector3.h"
#include "BsAABox.h"
#include "BsException.h"
#include "BsRenderSystem.h"

namespace BansheeEngine 
{
	IndexData::IndexData()
		:indexCount(0), indexStart(0)
	{ }

	IndexData::~IndexData()
	{ }
}
