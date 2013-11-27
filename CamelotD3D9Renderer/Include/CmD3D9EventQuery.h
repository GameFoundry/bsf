#pragma once

#include "CmD3D9Prerequisites.h"
#include "CmEventQuery.h"

namespace CamelotFramework
{
	/**
	 * @copydoc EventQuery
	 */
	class CM_D3D9_EXPORT D3D9EventQuery : public EventQuery
	{
	public:
		D3D9EventQuery();
		~D3D9EventQuery();

		/**
		 * @copydoc EventQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc EventQuery::isReady
		 */
		virtual bool isReady() const;

	private:
		IDirect3DQuery9* mQuery;
	};
}