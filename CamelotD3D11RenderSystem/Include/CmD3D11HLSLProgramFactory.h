#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmHighLevelGpuProgramManager.h"

namespace CamelotEngine
{
	class CM_D3D11_EXPORT D3D11HLSLProgramFactory : public HighLevelGpuProgramFactory
	{
	protected:
		static String sLanguageName;
	public:
		D3D11HLSLProgramFactory();
		~D3D11HLSLProgramFactory();

		const String& getLanguage(void) const;
		HighLevelGpuProgram* create(const String& source, const String& entryPoint, GpuProgramType gptype, GpuProgramProfile profile);
		HighLevelGpuProgram* create();
	};
}