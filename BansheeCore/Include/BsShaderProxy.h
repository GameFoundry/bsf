//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "BsCorePrerequisites.h"
#include "BsShader.h"

namespace BansheeEngine
{
	struct BS_CORE_EXPORT ShaderProxy
	{
		QueueSortType queueSortType;
		UINT32 queuePriority;
		bool separablePasses;

		Map<String, SHADER_DATA_PARAM_DESC> dataParams;
		Map<String, SHADER_OBJECT_PARAM_DESC> objectParams;
		Map<String, SHADER_PARAM_BLOCK_DESC> paramBlocks;
	};
}
