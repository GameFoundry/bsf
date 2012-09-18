#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	/**
	 * @brief	Abstract class that is to be specialized in converting
	 * 			a certain asset type into an engine usable resource. 
	 * 			(e.g. a .png file into an engine usable texture).
	 * 			
	 *			On initialization this class registers itself with the Importer module,
	 *			which delegates asset import calls to a specific importer.
	 */
	class SpecificImporter
	{
	public:
		SpecificImporter() {}
		virtual ~SpecificImporter() {}

		/**
		 * @brief	Returns the list of extensions this importer is responsible for importing.
		 *
		 * @return	A list of extensions, without a leading dot.
		 */
		virtual const std::vector<String>& extensions() const = 0;

		/**
		 * @brief	Imports the given file.
		 *
		 * @param	filePath	Pathname of the file, with file extension.
		 *
		 * @return	null if it fails, otherwise the loaded object.
		 */
		virtual ResourcePtr import(const String& filePath) = 0;
	};
}