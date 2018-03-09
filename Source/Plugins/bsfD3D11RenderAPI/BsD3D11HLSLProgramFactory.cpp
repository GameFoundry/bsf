//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#include "BsD3D11HLSLProgramFactory.h"
#include "BsD3D11GpuProgram.h"

namespace bs { namespace ct
{
	D3D11HLSLProgramFactory::D3D11HLSLProgramFactory()
	{ }

	D3D11HLSLProgramFactory::~D3D11HLSLProgramFactory()
	{ }

	SPtr<GpuProgram> D3D11HLSLProgramFactory::create(const GPU_PROGRAM_DESC& desc, GpuDeviceFlags deviceMask)
	{
		SPtr<GpuProgram> gpuProg;

		switch (desc.type)
		{
		case GPT_VERTEX_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuVertexProgram>(new (bs_alloc<D3D11GpuVertexProgram>())
				D3D11GpuVertexProgram(desc, deviceMask));
			break;
		case GPT_FRAGMENT_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuFragmentProgram>(new (bs_alloc<D3D11GpuFragmentProgram>())
				D3D11GpuFragmentProgram(desc, deviceMask));
			break;
		case GPT_HULL_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuHullProgram>(new (bs_alloc<D3D11GpuHullProgram>())
				D3D11GpuHullProgram(desc, deviceMask));
			break;
		case GPT_DOMAIN_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuDomainProgram>(new (bs_alloc<D3D11GpuDomainProgram>())
				D3D11GpuDomainProgram(desc, deviceMask));
			break;
		case GPT_GEOMETRY_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuGeometryProgram>(new (bs_alloc<D3D11GpuGeometryProgram>())
				D3D11GpuGeometryProgram(desc, deviceMask));
			break;
		case GPT_COMPUTE_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuComputeProgram>(new (bs_alloc<D3D11GpuComputeProgram>())
				D3D11GpuComputeProgram(desc, deviceMask));
			break;
		}

		if (gpuProg != nullptr)
			gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
	}

	SPtr<GpuProgram> D3D11HLSLProgramFactory::create(GpuProgramType type, GpuDeviceFlags deviceMask)
	{
		SPtr<GpuProgram> gpuProg;

		GPU_PROGRAM_DESC desc;
		desc.type = type;

		switch (type)
		{
		case GPT_VERTEX_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuVertexProgram>(new (bs_alloc<D3D11GpuVertexProgram>())
				D3D11GpuVertexProgram(desc, deviceMask));
			break;
		case GPT_FRAGMENT_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuFragmentProgram>(new (bs_alloc<D3D11GpuFragmentProgram>())
				D3D11GpuFragmentProgram(desc, deviceMask));
			break;
		case GPT_HULL_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuHullProgram>(new (bs_alloc<D3D11GpuHullProgram>())
				D3D11GpuHullProgram(desc, deviceMask));
			break;
		case GPT_DOMAIN_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuDomainProgram>(new (bs_alloc<D3D11GpuDomainProgram>())
				D3D11GpuDomainProgram(desc, deviceMask));
			break;
		case GPT_GEOMETRY_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuGeometryProgram>(new (bs_alloc<D3D11GpuGeometryProgram>())
				D3D11GpuGeometryProgram(desc, deviceMask));
			break;
		case GPT_COMPUTE_PROGRAM:
			gpuProg = bs_shared_ptr<D3D11GpuComputeProgram>(new (bs_alloc<D3D11GpuComputeProgram>())
				D3D11GpuComputeProgram(desc, deviceMask));
			break;
		}

		if (gpuProg != nullptr)
			gpuProg->_setThisPtr(gpuProg);

		return gpuProg;
	}
}}