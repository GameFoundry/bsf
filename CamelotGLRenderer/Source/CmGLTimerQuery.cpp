#include "CmGLTimerQuery.h"
#include "CmMath.h"

namespace CamelotFramework
{
	GLTimerQuery::GLTimerQuery()
		:mQueryStartObj(0), mQueryEndObj(0), 
		mTimeDelta(0.0f), mFinalized(false)
	{
		GLuint queries[2];
		glGenQueries(2, queries);

		mQueryStartObj = queries[0];
		mQueryEndObj = queries[1];
	}

	GLTimerQuery::~GLTimerQuery()
	{
		GLuint queries[2];
		queries[0] = mQueryStartObj;
		queries[1] = mQueryEndObj;

		glDeleteQueries(2, queries);
	}

	void GLTimerQuery::begin()
	{
		glQueryCounter(mQueryStartObj, GL_TIMESTAMP);

		setActive(true);
	}

	void GLTimerQuery::end()
	{
		glQueryCounter(mQueryEndObj, GL_TIMESTAMP);
	}

	bool GLTimerQuery::isReady() const
	{
		GLint done = 0;
		glGetQueryObjectiv(mQueryEndObj, GL_QUERY_RESULT_AVAILABLE, &done);

		return done == GL_TRUE;
	}

	float GLTimerQuery::getTimeMs() 
	{
		if(!mFinalized && isReady())
		{
			finalize();
		}

		return mTimeDelta;
	}

	void GLTimerQuery::finalize()
	{
		mFinalized = true;

		GLuint64 timeStart;
		GLuint64 timeEnd;

		glGetQueryObjectui64v(mQueryStartObj, GL_QUERY_RESULT, &timeStart);
		glGetQueryObjectui64v(mQueryEndObj, GL_QUERY_RESULT, &timeEnd);

		mTimeDelta = (timeEnd - timeStart) / 1000000.0f;
	}
}