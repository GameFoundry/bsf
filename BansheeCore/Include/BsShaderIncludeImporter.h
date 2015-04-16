#pragma once

#include "BsCorePrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Importer using for importing GPU program (i.e. shader) include files.
	 * 			Include files are just text files ending with ".bsi" extension.
	 */
	class BS_CORE_EXPORT ShaderIncludeImporter : public SpecificImporter
	{
	public:
		ShaderIncludeImporter();
		virtual ~ShaderIncludeImporter();

		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);
	private:
	};
}