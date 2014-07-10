//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsGpuProgramManager.h"

namespace BansheeEngine
{
	/**
	 * @brief	Factory class that deals with creating GLSL GPU programs.
	 */
    class BS_RSGL_EXPORT GLSLProgramFactory : public GpuProgramFactory
    {
    public:
		/**
		 * @copydoc	GpuProgramFactory::getLanguage
		 */
		const String& getLanguage() const;

		/**
		 * @copydoc	GpuProgramFactory::getLanguage(const String&, const String&, GpuProgramType,
		 *			GpuProgramProfile, const Vector<HGpuProgInclude>*, bool)
		 */
        GpuProgramPtr create(const String& source, const String& entryPoint, GpuProgramType gptype, 
			GpuProgramProfile profile, const Vector<HGpuProgInclude>* includes, bool requireAdjacency);

		/**
		 * @copydoc	GpuProgramFactory::create(GpuProgramType)
		 */
		GpuProgramPtr create(GpuProgramType type);

	protected:
		static const String LANGUAGE_NAME;
    };
}