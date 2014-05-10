#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmGpuProgramManager.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11HLSLProgramFactory : public GpuProgramFactory
	{
	public:
		D3D11HLSLProgramFactory();
		~D3D11HLSLProgramFactory();

		const String& getLanguage(void) const;
		GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requireAdjacencyInfo);
		GpuProgramPtr create();

	protected:
		static const String LANGUAGE_NAME;
	};
}