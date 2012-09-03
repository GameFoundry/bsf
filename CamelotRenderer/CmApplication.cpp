#include "CmApplication.h"

namespace CamelotEngine
{
	Application& gApplication()
	{
		static Application application;
		return application;
	}
}