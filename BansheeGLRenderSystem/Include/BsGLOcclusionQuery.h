#pragma once

#include "BsGLPrerequisites.h"
#include "BsOcclusionQuery.h"

namespace BansheeEngine
{
	/**
	 * @brief	OpenGL implementation of an occlusion query.
	 */
	class BS_RSGL_EXPORT GLOcclusionQuery : public OcclusionQuery
	{
	public:
		GLOcclusionQuery(bool binary);
		~GLOcclusionQuery();

		/**
		 * @copydoc OcclusionQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc OcclusionQuery::end
		 */
		virtual void end();

		/**
		 * @copydoc OcclusionQuery::isReady
		 */
		virtual bool isReady() const;

		/**
		 * @copydoc OcclusionQuery::getNumFragments
		 */
		virtual UINT32 getNumSamples();

	private:
		friend class QueryManager;

		/**
		 * @brief	Processes query results and saves them for later use. To be called
		 *			when query has completed.
		 */
		void finalize();

	private:
		GLuint mQueryObj;
		bool mFinalized;
		bool mEndIssued;

		UINT32 mNumSamples;
	};
}