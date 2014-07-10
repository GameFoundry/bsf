//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsSpecificImporter.h"

namespace BansheeEngine
{
	/**
	 * @brief	Importer using for importing GPU program (i.e. shader) include files.
	 * 			Include files are just text files ending with ".gpuproginc" extension.
	 */
	class BS_CORE_EXPORT GpuProgIncludeImporter : public SpecificImporter
	{
	public:
		GpuProgIncludeImporter();
		virtual ~GpuProgIncludeImporter();

		/** @copydoc SpecificImporter::isExtensionSupported */
		virtual bool isExtensionSupported(const WString& ext) const;

		/** @copydoc SpecificImporter::isMagicNumberSupported */
		virtual bool isMagicNumberSupported(const UINT8* magicNumPtr, UINT32 numBytes) const; 

		/** @copydoc SpecificImporter::import */
		virtual ResourcePtr import(const Path& filePath, ConstImportOptionsPtr importOptions);
	private:
	};
}