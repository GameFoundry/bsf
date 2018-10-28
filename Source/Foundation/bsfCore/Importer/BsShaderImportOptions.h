//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsCorePrerequisites.h"
#include "Importer/BsImportOptions.h"

namespace bs
{
	/** @addtogroup Importer
	 *  @{
	 */

	/** Supported types of low-level shading languages. */
	enum class ShadingLanguageFlag
	{
		/** High level shading language used by DirectX backend. */
		HLSL = 1 << 0, 
		/** OpenGL shading language. */
		GLSL = 1 << 1, 
		/** Variant of GLSL used for Vulkan. */
		VKSL = 1 << 2,
		/** Helper entry that includes all languages. */
		All = HLSL | GLSL | VKSL
	};

	using ShadingLanguageFlags = Flags<ShadingLanguageFlag>;
	BS_FLAGS_OPERATORS(ShadingLanguageFlag)

	/** Contains import options you may use to control how is a shader imported. */
	class BS_CORE_EXPORT ShaderImportOptions : public ImportOptions
	{
	public:
		/** Returns a modifiable list of defines that will control shader compilation. */
		UnorderedMap<String, String>& getDefines() { return mDefines; }

		/** Returns a list of defines that will control shader compilation. */
		const UnorderedMap<String, String>& getDefines() const { return mDefines; }

		/** 
		 * Returns a modifiable set of flags that control which shading languages should the BSL shader be converted into.
		 * This ultimately controls on which render backends it will be able to run on.
		 */
		ShadingLanguageFlags& getLanguages() { return mLanguages; }

		/** 
		 * Returns a set of flags that control which shading languages should the BSL shader be converted into.
		 * This ultimately controls on which render backends it will be able to run on.
		 */
		const ShadingLanguageFlags& getLanguages() const { return mLanguages; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class ShaderImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		RTTITypeBase* getRTTI() const override;

	private:
		UnorderedMap<String, String> mDefines;
		ShadingLanguageFlags mLanguages = ShadingLanguageFlag::All;
	};

	/** @} */
}