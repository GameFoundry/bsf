#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmGpuProgramManager.h"

namespace BansheeEngine
{
    /** Factory class for D3D9 HLSL programs. */
    class CM_D3D9_EXPORT D3D9HLSLProgramFactory : public GpuProgramFactory
    {
    public:
        D3D9HLSLProgramFactory();
        ~D3D9HLSLProgramFactory();

		const String& getLanguage() const;

        GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requiresAdjacency);

		GpuProgramPtr create(GpuProgramType type);

	protected:
		static String LANGUAGE_NAME;
    };
}