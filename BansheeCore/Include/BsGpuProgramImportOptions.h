//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsImportOptions.h"
#include "BsGpuProgram.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains import options you may use to control how is a GPU program (i.e. shader)
	 * 			file imported.
	 */
	class BS_CORE_EXPORT GpuProgramImportOptions : public ImportOptions
	{
	public:
		GpuProgramImportOptions();

		/**
		 * @brief	Sets the name of the GPU program entry point method (e.g. "main").
		 */
		void setEntryPoint(const String& entryPoint) { mEntryPoint = entryPoint; }

		/**
		 * @brief	Sets the language the GPU program is written in (e.g. HLSL9, GLSL, HLSL11).
		 */
		void setLanguage(const String& language) { mLanguage = language; }

		/**
		 * @brief	Sets GPU program profile. Determines minimal feature-set the GPU program
		 * 			requires in order to function.
		 */
		void setProfile(GpuProgramProfile profile) { mProfile = profile; }

		/**
		 * @brief	Sets GPU program type (e.g. vertex, pixel, etc.).
		 */
		void setType(GpuProgramType type) { mType = type; }

		/**
		 * @brief	Sets optional GPU program include files which may be used
		 * 			for sharing code across multiple programs.
		 */
		void setIncludes(const Vector<HGpuProgInclude>& includes) { mIncludes = includes; }

		/**
		 * @brief	Gets the name of the GPU program entry point method (e.g. "main").
		 */
		const String& getEntryPoint() const { return mEntryPoint; }

		/**
		 * @brief	Gets the language the GPU program is written in (e.g. HLSL9, GLSL, HLSL11).
		 */
		const String& getLanguage() const { return mLanguage; }

		/**
		 * @brief	Gets GPU program profile. Determines minimal feature-set the GPU program
		 * 			requires in order to function.
		 */
		GpuProgramProfile getProfile() const { return mProfile; }

		/**
		 * @brief	Gets GPU program type (e.g. vertex, pixel, etc.).
		 */
		GpuProgramType getType() const { return mType; }

		/**
		 * @brief	Gets optional GPU program include files which may be used
		 * 			for sharing code across multiple programs.
		 */
		const Vector<HGpuProgInclude>& getIncludes() const { return mIncludes; }

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class GpuProgramImportOptionsRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;

	private:
		String mEntryPoint;
		String mLanguage;
		GpuProgramProfile mProfile;
		GpuProgramType mType;
		Vector<HGpuProgInclude> mIncludes;
	};
}