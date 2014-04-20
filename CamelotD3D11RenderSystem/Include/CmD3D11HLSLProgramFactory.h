#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHighLevelGpuProgramManager.h"

namespace BansheeEngine
{
	class CM_D3D11_EXPORT D3D11HLSLProgramFactory : public HighLevelGpuProgramFactory
	{
	protected:
		static String sLanguageName;
	public:
		D3D11HLSLProgramFactory();
		~D3D11HLSLProgramFactory();

		const String& getLanguage(void) const;
		HighLevelGpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>::type* includes);
		HighLevelGpuProgramPtr create();
	};
}