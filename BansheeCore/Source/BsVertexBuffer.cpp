//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsVertexBuffer.h"
#include "BsColor.h"
#include "BsException.h"
#include "BsHardwareBufferManager.h"
#include "BsRenderSystem.h"
#include "BsVertexDeclarationRTTI.h"

namespace BansheeEngine 
{
    VertexBuffer::VertexBuffer(UINT32 vertexSize,  UINT32 numVertices, GpuBufferUsage usage, bool useSystemMemory) 
        : HardwareBuffer(usage, useSystemMemory), mNumVertices(numVertices), mVertexSize(vertexSize)
    {
        mSizeInBytes = mVertexSize * numVertices;
    }
}
