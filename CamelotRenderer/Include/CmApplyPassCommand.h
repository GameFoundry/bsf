#pragma once

#include "CmPrerequisites.h"

namespace CamelotEngine
{
	class ApplyPassCommand
	{
	public:
		void setPass(PassPtr& pass) { mPass = pass; }
		PassPtr getPass() const { return mPass; }

		void setPassParameters(PassParametersPtr& passParameters) { mPassParameters = passParameters; }
		PassParametersPtr getPassParameters() const { return mPassParameters; }

	private:
		PassPtr mPass;
		PassParametersPtr mPassParameters;
	};
}