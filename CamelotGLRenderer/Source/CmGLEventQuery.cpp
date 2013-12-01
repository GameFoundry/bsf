#include "CmGLEventQuery.h"

namespace CamelotFramework
{
	GLEventQuery::GLEventQuery()
		:mQueryObj(0)
	{
		glGenQueries(1, &mQueryObj);
	}

	GLEventQuery::~GLEventQuery()
	{
		glDeleteQueries(1, &mQueryObj);
	}

	void GLEventQuery::begin()
	{
		glQueryCounter(mQueryObj, GL_TIMESTAMP);
		setActive(true);
	}

	bool GLEventQuery::isReady() const
	{
		GLint done = 0;
		glGetQueryObjectiv(mQueryObj, GL_QUERY_RESULT_AVAILABLE, &done);

		return done == GL_TRUE;
	}
}