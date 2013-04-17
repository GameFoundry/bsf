#pragma once

#include "CmGLPrerequisites.h"

namespace CamelotFramework
{
	struct GLSLProgramPipeline
	{
		GLuint glHandle;
	};

	class GLSLProgramPipelineManager
	{
	public:
		const GLSLProgramPipeline* getPipeline(GLSLGpuProgram* vertexProgram, GLSLGpuProgram* fragmentProgram, 
			GLSLGpuProgram* geometryProgram, GLSLGpuProgram* hullProgram, GLSLGpuProgram* domainProgram);

	private:
		struct ProgramPipelineKey
		{
			UINT32 vertexProgKey;
			UINT32 fragmentProgKey;
			UINT32 geometryProgKey;
			UINT32 hullProgKey;
			UINT32 domainProgKey;
		};

		class ProgramPipelineKeyHashFunction 
		{
		public:
			::std::size_t operator()(const ProgramPipelineKey &key) const;
		};

		class ProgramPipelineKeyEqual 
		{
		public:
			bool operator()(const ProgramPipelineKey &a, const ProgramPipelineKey &b) const;
		};

		typedef std::unordered_map<ProgramPipelineKey, GLSLProgramPipeline, ProgramPipelineKeyHashFunction, ProgramPipelineKeyEqual> ProgramPipelineMap;
		ProgramPipelineMap mPipelines;
	};
}