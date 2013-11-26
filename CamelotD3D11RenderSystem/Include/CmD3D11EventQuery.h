#pragma once

#include "CmD3D11Prerequisites.h"
#include "CmEventQuery.h"

namespace CamelotFramework
{
	/**
	 * @copydoc EventQuery
	 */
	class CM_D3D11_EXPORT D3D11EventQuery : public EventQuery
	{
	public:
		D3D11EventQuery();
		~D3D11EventQuery();

		/**
		 * @copydoc EventQuery::begin
		 */
		virtual void begin();

		/**
		 * @copydoc EventQuery::isReady
		 */
		virtual bool isReady() const;

	private:
		ID3D11Query* mQuery;
		ID3D11DeviceContext* mContext;
		bool mInitialized;
	};
}