#pragma once

#include "CmPrerequisitesUtil.h"

namespace CamelotEngine
{
	class CM_UTILITY_EXPORT FileSystem
	{
	public:
		static DataStreamPtr open(const String& fullPath, bool readOnly = true);

		static DataStreamPtr create(const String& fullPath);

		static void remove(const String& fullPath);
	};
}