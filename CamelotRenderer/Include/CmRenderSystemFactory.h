#pragma once

#include <string>
#include <memory>

namespace CamelotEngine
{
	class RenderSystem;
}

namespace CamelotEngine
{
	typedef std::shared_ptr<RenderSystem> RenderSystemPtr;

	class RenderSystemFactory
	{
	public:
		virtual RenderSystemPtr create() = 0;
		virtual const std::string& name() const = 0;
	};
}