#include "CmGLEventQuery.h"

namespace CamelotFramework
{
	GLEventQuery::GLEventQuery()
		:mQueryObj(0), mInitialized(false)
	{

	}

	GLEventQuery::~GLEventQuery()
	{
		if(mInitialized)
		{
			GLuint queries[1];
			queries[0] = mQueryObj;

			glDeleteQueries(1, queries);
		}
	}

	void GLEventQuery::begin()
	{
		GLuint queries[1];
		queries[0] = mQueryObj;

		glGenQueries(1, queries);
		glQueryCounter(mQueryObj, GL_TIMESTAMP);

		mInitialized = true;
	}

	bool GLEventQuery::isReady() const
	{
		GLint done = 0;
		glGetQueryObjectiv(mQueryObj, GL_QUERY_RESULT_AVAILABLE, &done);

		return done == GL_TRUE;
	}
}