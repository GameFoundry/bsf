#pragma once

#include <memory>

namespace CamelotEngine
{
	class Application
	{
		public:
			Application();

			void startUp();
			void shutDown();
	};

	Application& gApplication();
}