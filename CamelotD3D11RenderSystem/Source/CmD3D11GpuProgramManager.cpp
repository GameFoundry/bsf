#include "CmD3D11GpuProgramManager.h"
#include "CmD3D11GpuProgram.h"

namespace CamelotEngine
{
	D3D11GpuProgramManager::D3D11GpuProgramManager(D3D11Device& device)
		:mDevice(device)
	{ }

	D3D11GpuProgramManager::~D3D11GpuProgramManager()
	{ }

	GpuProgram* D3D11GpuProgramManager::create(const String& source, const String& entryPoint, 
		const String& language, GpuProgramType gptype, GpuProgramProfile profile)
	{
		switch(gptype)
		{
		case GPT_VERTEX_PROGRAM:
			return new D3D11GpuVertexProgram(profile);
		case GPT_FRAGMENT_PROGRAM:
			return new D3D11GpuFragmentProgram(profile);
		case GPT_HULL_PROGRAM:
			return new D3D11GpuHullProgram(profile);
		case GPT_DOMAIN_PROGRAM:
			return new D3D11GpuDomainProgram(profile);
		case GPT_GEOMETRY_PROGRAM:
			return new D3D11GpuGeometryProgram(profile);
		}
		
		return nullptr;
	}
}