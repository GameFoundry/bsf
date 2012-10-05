#pragma once

#include "CmPrerequisites.h"
#include "CmResource.h"

namespace CamelotEngine
{
	class Mesh : public Resource
	{
		virtual void load();

		/************************************************************************/
		/* 								SERIALIZATION                      		*/
		/************************************************************************/
	public:
		friend class MeshST;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};
}