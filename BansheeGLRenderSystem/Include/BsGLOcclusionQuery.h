#pragma once

#include "BsGLPrerequisites.h"
#include "BsOcclusionQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc OcclusionQuery
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

		GLuint mQueryObj;
		bool mFinalized;

		UINT32 mNumSamples;

		void finalize();
	};
}