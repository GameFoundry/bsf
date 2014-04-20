#pragma once

#include "CmPrerequisites.h"
#include "CmSpecificImporter.h"

namespace BansheeEngine
{
	class CM_EXPORT GpuProgIncludeImporter : public SpecificImporter
	{
	public:
		GpuProgIncludeImporter();
		virtual ~GpuProgIncludeImporter();

		/** Inherited from SpecificImporter */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** Inherited from SpecificImporter */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** Inherited from SpecificImporter */
		virtual ResourcePtr import(const WString& filePath, ConstImportOptionsPtr importOptions);
	private:
	};
}