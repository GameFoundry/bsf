#pragma once

#include <string>
#include <memory>

namespace CamelotFramework
{
	class RenderSystem;
}

namespace CamelotFramework
{
	typedef std::shared_ptr<RenderSystem> RenderSystemPtr;

	class RenderSystemFactory
	{
	public:
		virtual void create() = 0;
		virtual const std::string& name() const = 0;
	};
}