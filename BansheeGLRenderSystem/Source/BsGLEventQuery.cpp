//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#include "BsGLEventQuery.h"
#include "BsRenderStats.h"

namespace BansheeEngine
{
	GLEventQuery::GLEventQuery()
		:mQueryObj(0)
	{
		glGenQueries(1, &mQueryObj);
		BS_INC_RENDER_STAT_CAT(ResCreated, RenderStatObject_Query);
	}

	GLEventQuery::~GLEventQuery()
	{
		glDeleteQueries(1, &mQueryObj);
		BS_INC_RENDER_STAT_CAT(ResDestroyed, RenderStatObject_Query);
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