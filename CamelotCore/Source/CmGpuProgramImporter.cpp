#include "CmGpuProgramImporter.h"
#include "CmPath.h"
#include "CmDataStream.h"
#include "CmFileSystem.h"
#include "CmGpuProgramImportOptions.h"
#include "CmHighLevelGpuProgram.h"

namespace CamelotFramework
{
	bool GpuProgramImporter::isExtensionSupported(const String& ext) const
	{
		if(ext == "gpuprog")
			return true;

		return false;
	}

	bool GpuProgramImporter::isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const
	{
		// Gpu program files are plain-text so no magic number
		return true;
	}

	HResource GpuProgramImporter::import(const String& filePath, ConstImportOptionsPtr importOptions)
	{
		String ext = Path::getExtension(filePath);
		ext = ext.substr(1, ext.size() - 1); // Remove the .

		DataStreamPtr stream = FileSystem::open(filePath);
		String shaderSource = stream->getAsString();

		const GpuProgramImportOptions* gpuProgImportOptions = static_cast<const GpuProgramImportOptions*>(importOptions.get());

		String entryPoint = gpuProgImportOptions->getEntryPoint();
		GpuProgramProfile profile = gpuProgImportOptions->getProfile();
		String language = gpuProgImportOptions->getLanguage();
		GpuProgramType gptype = gpuProgImportOptions->getType();
		vector<HGpuProgInclude>::type includes = gpuProgImportOptions->getIncludes();

		HHighLevelGpuProgram gpuProgram = HighLevelGpuProgram::create(shaderSource, entryPoint, language, gptype, profile, &includes);
		gpuProgram.waitUntilLoaded();

		return gpuProgram;
	}

	ImportOptionsPtr GpuProgramImporter::createImportOptions() const
	{
		return cm_shared_ptr<GpuProgramImportOptions, PoolAlloc>();
	}
}