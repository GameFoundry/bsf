#include "CmGLOcclusionQuery.h"
#include "CmMath.h"

namespace BansheeEngine
{
	GLOcclusionQuery::GLOcclusionQuery(bool binary)
		:OcclusionQuery(binary), mQueryObj(0), mNumFragments(0), mFinalized(false)
	{
		glGenQueries(1, &mQueryObj);
	}

	GLOcclusionQuery::~GLOcclusionQuery()
	{
		glDeleteQueries(1, &mQueryObj);
	}

	void GLOcclusionQuery::begin()
	{
		glBeginQuery(mBinary ? GL_ANY_SAMPLES_PASSED : GL_SAMPLES_PASSED, mQueryObj);

		mNumFragments = 0;
		setActive(true);
	}

	void GLOcclusionQuery::end()
	{
		glEndQuery(mBinary ? GL_ANY_SAMPLES_PASSED : GL_SAMPLES_PASSED);
	}

	bool GLOcclusionQuery::isReady() const
	{
		GLint done = 0;
		glGetQueryObjectiv(mQueryObj, GL_QUERY_RESULT_AVAILABLE, &done);

		return done == GL_TRUE;
	}

	UINT32 GLOcclusionQuery::getNumFragments()
	{
		if (!mFinalized && isReady())
		{
			finalize();
		}

		return mNumFragments;
	}

	void GLOcclusionQuery::finalize()
	{
		mFinalized = true;

		if (mBinary)
		{
			GLboolean anyPassed = GL_FALSE;
			glGetQueryObjectuiv(mQueryObj, GL_QUERY_RESULT_ARB, (GLuint*)&anyPassed);

			mNumFragments = anyPassed == GL_TRUE ? 1 : 0;
		}
		else
		{
			GLuint numFragments = 0;
			glGetQueryObjectuiv(mQueryObj, GL_QUERY_RESULT_ARB, (GLuint*)&numFragments);

			mNumFragments = (UINT32)numFragments;
		}
	}
}
