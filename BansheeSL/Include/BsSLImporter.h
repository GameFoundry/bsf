#pragma once

#include "BsSLPrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	* @brief	Importer using for importing a shader written using the BSL syntax.
	* 			Shader files are plain text files ending with ".bsl" extension.
	*/
	class BS_SL_EXPORT SLImporter : public SpecificImporter
	{
	public:
		SLImporter();
		virtual ~SLImporter();

		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const override;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const override;

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions) override;
	};
}