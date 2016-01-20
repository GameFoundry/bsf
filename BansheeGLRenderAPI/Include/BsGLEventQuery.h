//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsGLPrerequisites.h"
#include "BsEventQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc EventQuery
	 */
	class BS_RSGL_EXPORT GLEventQuery : public EventQuery
	{
	public:
		GLEventQuery();
		~GLEventQuery();

		/**
		 * @copydoc EventQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc EventQuery::isReady
		 */
		virtual bool isReady() const;

	private:
		GLuint mQueryObj;
	};
}