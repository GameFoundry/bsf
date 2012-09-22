#pragma once

#include "CmPrerequisitesUtil.h"
#include <boost/uuid/uuid.hpp>

namespace CamelotEngine
{
	typedef boost::uuids::uuid UUID;

	CM_SERIALIZABLE_SIMPLE_TYPE(UUID, 51);
}