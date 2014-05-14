#pragma once

#include "CmGLPrerequisites.h"
#include "CmOcclusionQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc OcclusionQuery
	 */
	class CM_RSGL_EXPORT GLOcclusionQuery : public OcclusionQuery
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
		virtual UINT32 getNumFragments();

	private:
		friend class QueryManager;

		GLuint mQueryObj;
		bool mFinalized;

		UINT32 mNumFragments;

		void finalize();
	};
}