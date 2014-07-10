//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Importer for GPU programs (i.e. shaders). File must end with ".gpuprog" extension,
	 * 			and actual type of the program is determined via import options.
	 */
	class BS_CORE_EXPORT GpuProgramImporter : public SpecificImporter
	{
	public:
		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);

		/** @copydoc SpecificImporter::createImportOptions */
		virtual ImportOptionsPtr createImportOptions() const;
	};
}