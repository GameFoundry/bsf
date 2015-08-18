#pragma once

#include "BsCorePrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Importer using for importing Prefab resources.
	 * 			Prefab resources are serialized engine objects ending in ".prefab" extension.
	 */
	class BS_CORE_EXPORT PrefabImporter : public SpecificImporter
	{
	public:
		PrefabImporter();
		virtual ~PrefabImporter();

		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const override;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const override;

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions) override;
	};
}