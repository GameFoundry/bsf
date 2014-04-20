#pragma once

#include "CmPrerequisites.h"
#include "CmSpecificImporter.h"

namespace BansheeEngine
{
	class CM_EXPORT GpuProgramImporter : public SpecificImporter
	{
	public:
		/**
		 * @copydoc SpecificImporter::isExtensionSupported().
		 */
		virtual bool isExtensionSupported(const WString& ext) const;

		/**
		 * @copydoc SpecificImporter::isMagicNumberSupported().
		 */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/**
		 * @copydoc SpecificImporter::import().
		 */
		virtual ResourcePtr import(const WString& filePath, ConstImportOptionsPtr importOptions);

		/**
		 * @copydoc SpecificImporter::createImportOptions().
		 */
		virtual ImportOptionsPtr createImportOptions() const;
	};
}