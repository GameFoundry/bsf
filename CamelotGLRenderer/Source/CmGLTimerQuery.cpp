#include "CmGLTimerQuery.h"

namespace CamelotFramework
{
	GLTimerQuery::GLTimerQuery()
		:mQueryObj(0), mInitialized(false)
	{

	}

	GLTimerQuery::~GLTimerQuery()
	{
		if(mInitialized)
		{
			// TODO
		}
	}

	void GLTimerQuery::begin()
	{
		// TODO

		mInitialized = true;
	}

	void GLTimerQuery::end()
	{
		// TODO
	}

	bool GLTimerQuery::isReady() const
	{
		// TODO
		return false;
	}

	UINT64 GLTimerQuery::getTimeMs() const
	{
		// TODO
		return 0;
	}
}