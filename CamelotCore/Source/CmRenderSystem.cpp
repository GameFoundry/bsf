/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2011 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
// RenderSystem implementation
// Note that most of this class is abstract since
//  we cannot know how to implement the behaviour without
//  being aware of the 3D API. However there are a few
//  simple functions which can have a base implementation

#include "CmRenderSystem.h"

#include "CmCoreThreadAccessor.h"
#include "CmViewport.h"
#include "CmException.h"
#include "CmRenderTarget.h"
#include "CmRenderWindow.h"
#include "CmPixelBuffer.h"
#include "CmOcclusionQuery.h"
#include "CmGpuResource.h"
#include "CmCoreThread.h"
#include "CmMesh.h"
#include "boost/bind.hpp"

#include "CmProfiler.h"

namespace CamelotFramework {

    static const TexturePtr sNullTexPtr;

    RenderSystem::RenderSystem()
        : mCullingMode(CULL_COUNTERCLOCKWISE)
        , mDisabledTexUnitsFrom(0)
        , mVertexProgramBound(false)
		, mGeometryProgramBound(false)
        , mFragmentProgramBound(false)
		, mClipPlanesDirty(true)
		, mCurrentCapabilities(nullptr)
    {
    }

    RenderSystem::~RenderSystem()
    {
		// Base classes need to call virtual destroy_internal method (queue it on core thread)

		cm_delete(mCurrentCapabilities);
		mCurrentCapabilities = nullptr;
    }

	RenderWindowPtr RenderSystem::initialize(const RENDER_WINDOW_DESC& primaryWindowDesc)
	{
		mPrimaryWindowDesc = primaryWindowDesc;

		AsyncOp op = gCoreThread().queueReturnCommand(boost::bind(&RenderSystem::initialize_internal, this, _1), true);
		return op.getReturnValue<RenderWindowPtr>();
	}

	void RenderSystem::initialize_internal(AsyncOp& asyncOp)
	{
		THROW_IF_NOT_CORE_THREAD;

		mVertexProgramBound = false;
		mGeometryProgramBound = false;
		mFragmentProgramBound = false;
	}

	void RenderSystem::destroy_internal()
	{
		mActiveRenderTarget = nullptr;
	}

	const RenderSystemCapabilities* RenderSystem::getCapabilities(void) const 
	{ 
		THROW_IF_NOT_CORE_THREAD;

		return mCurrentCapabilities; 
	}

	const DriverVersion& RenderSystem::getDriverVersion(void) const 
	{ 
		THROW_IF_NOT_CORE_THREAD;

		return mDriverVersion; 
	}

    void RenderSystem::disableTextureUnit(GpuProgramType gptype, UINT16 texUnit)
    {
		THROW_IF_NOT_CORE_THREAD;

        setTexture(gptype, texUnit, false, sNullTexPtr);
    }

	void RenderSystem::addClipPlane (const Plane &p)
	{
		THROW_IF_NOT_CORE_THREAD;

		mClipPlanes.push_back(p);
		mClipPlanesDirty = true;
	}

	void RenderSystem::addClipPlane (float A, float B, float C, float D)
	{
		THROW_IF_NOT_CORE_THREAD;

		addClipPlane(Plane(A, B, C, D));
	}

	void RenderSystem::setClipPlanes(const PlaneList& clipPlanes)
	{
		THROW_IF_NOT_CORE_THREAD;

		if (clipPlanes != mClipPlanes)
		{
			mClipPlanes = clipPlanes;
			mClipPlanesDirty = true;
		}
	}

	void RenderSystem::resetClipPlanes()
	{
		THROW_IF_NOT_CORE_THREAD;

		if (!mClipPlanes.empty())
		{
			mClipPlanes.clear();
			mClipPlanesDirty = true;
		}
	}

	void RenderSystem::bindGpuProgram(HGpuProgram prg)
	{
		THROW_IF_NOT_CORE_THREAD;

		switch(prg->getBindingDelegate()->getType())
		{
		case GPT_VERTEX_PROGRAM:
			// mark clip planes dirty if changed (programmable can change space)
			if (!mVertexProgramBound && !mClipPlanes.empty())
				mClipPlanesDirty = true;

			mVertexProgramBound = true;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mGeometryProgramBound = true;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mFragmentProgramBound = true;
			break;
		}
	}

	void RenderSystem::unbindGpuProgram(GpuProgramType gptype)
	{
		THROW_IF_NOT_CORE_THREAD;

		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			// mark clip planes dirty if changed (programmable can change space)
			if (mVertexProgramBound && !mClipPlanes.empty())
				mClipPlanesDirty = true;
			mVertexProgramBound = false;
			break;
		case GPT_GEOMETRY_PROGRAM:
			mGeometryProgramBound = false;
			break;
		case GPT_FRAGMENT_PROGRAM:
			mFragmentProgramBound = false;
			break;
		}
	}

	bool RenderSystem::isGpuProgramBound(GpuProgramType gptype)
	{
		THROW_IF_NOT_CORE_THREAD;

	    switch(gptype)
	    {
        case GPT_VERTEX_PROGRAM:
            return mVertexProgramBound;
        case GPT_GEOMETRY_PROGRAM:
            return mGeometryProgramBound;
        case GPT_FRAGMENT_PROGRAM:
            return mFragmentProgramBound;
	    }

        return false;
	}

	void RenderSystem::render(const MeshPtr& mesh, UINT32 submeshIdx)
	{
		THROW_IF_NOT_CORE_THREAD;

		gProfiler().beginSample("render");

		// sort out clip planes
		// have to do it here in case of matrix issues
		if (mClipPlanesDirty)
		{
			setClipPlanesImpl(mClipPlanes);
			mClipPlanesDirty = false;
		}

		const SubMesh& subMesh = mesh->getSubMesh(submeshIdx);

		setVertexDeclaration(subMesh.vertexData->vertexDeclaration);
		auto vertexBuffers = subMesh.vertexData->getBuffers();

		for(auto iter = vertexBuffers.begin(); iter != vertexBuffers.end() ; ++iter)
			setVertexBuffer(iter->first, iter->second);

		setDrawOperation(subMesh.drawOp);

		if (subMesh.useIndexes)
		{
			setIndexBuffer(subMesh.indexData->indexBuffer);
			drawIndexed(subMesh.indexData->indexStart, subMesh.indexData->indexCount, subMesh.vertexData->vertexCount);
		}
		else
			draw(subMesh.vertexData->vertexCount);

		gProfiler().endSample("render");
	}

	void RenderSystem::swapBuffers(RenderTargetPtr target)
	{
		THROW_IF_NOT_CORE_THREAD;

		target->swapBuffers();
	}

	void RenderSystem::writeSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, const GpuResourceDataPtr& data, bool discardEntireBuffer, AsyncOp& asyncOp)
	{
		THROW_IF_NOT_CORE_THREAD;

		gProfiler().beginSample("writeSubresource");

		resource->writeSubresource(subresourceIdx, *data, discardEntireBuffer);

		gProfiler().endSample("writeSubresource");

		gProfiler().beginSample("writeSubresourceB");

		data->unlock();
		asyncOp.completeOperation();

		gProfiler().endSample("writeSubresourceB");
	}

	void RenderSystem::readSubresource(GpuResourcePtr resource, UINT32 subresourceIdx, GpuResourceDataPtr& data, AsyncOp& asyncOp)
	{
		THROW_IF_NOT_CORE_THREAD;

		gProfiler().beginSample("readSubresource");

		resource->readSubresource(subresourceIdx, *data);
		data->unlock();
		asyncOp.completeOperation();

		gProfiler().endSample("readSubresource");
	}
}