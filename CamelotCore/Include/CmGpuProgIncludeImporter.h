#pragma once

#include "CmPrerequisites.h"
#include "CmSpecificImporter.h"

namespace CamelotFramework
{
	class CM_EXPORT GpuProgIncludeImporter : public SpecificImporter
	{
	public:
		GpuProgIncludeImporter();
		virtual ~GpuProgIncludeImporter();

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const String& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual HResource import(const String& filePath, ConstImportOptionsPtr importOptions);
	private:
	};
}