#pragma once

#include "BsCorePrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	/**
	 * @brief	Interface that provides a method for finding a shader include resource
	 *			based on the name of the include that was provided in a shader file.
	 */
	class BS_CORE_EXPORT IShaderIncludeHandler
	{
	public:
		virtual ~IShaderIncludeHandler() { }

		/**
		 * @brief	Attempts to find a shader include resource based on its name.
		 */
		virtual HShaderInclude findInclude(const String& name) const = 0;
	};

	/**
	 * @brief	Implements shader include finding by converting the shader 
	 *			include name into a path that the resource will be loaded from.
	 */
	class BS_CORE_EXPORT DefaultShaderIncludeHandler : public IShaderIncludeHandler
	{
	public:
		/**
		 * @copydoc	IShaderIncludeHandler::findInclude
		 */
		virtual HShaderInclude findInclude(const String& name) const override;
	};

	/**
	 * @brief	A global manager that handles various shader specific operations.
	 */
	class BS_CORE_EXPORT ShaderManager : public Module <ShaderManager>
	{
	public:
		ShaderManager(const ShaderIncludeHandlerPtr& handler) { mIncludeHandler = handler; }

		/**
		 * @brief	Attempts to find a shader include based on the include name.
		 *
		 * @note	The name is usually a path to the resource relative to the working folder,
		 *			but can be other things depending on active handler.
		 */
		HShaderInclude findInclude(const String& name) const;

		/**
		 * @brief	Changes the active include handler that determines how is
		 *			a shader include name mapped to the actual resource.
		 */
		void setIncludeHandler(const ShaderIncludeHandlerPtr& handler) { mIncludeHandler = handler; }

	private:
		ShaderIncludeHandlerPtr mIncludeHandler;
	};
}