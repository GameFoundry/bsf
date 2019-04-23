//************************************ bs::framework - Copyright 2018 Marko Pintera **************************************//
//*********** Licensed under the MIT license. See LICENSE.md for full terms. This notice is not to be removed. ***********//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "RenderAPI/BsEventQuery.h"

namespace bs { namespace ct
{
	/** @addtogroup D3D11
	 *  @{
	 */

	/** @copydoc EventQuery */
	class D3D11EventQuery : public EventQuery
	{
	public:
		D3D11EventQuery(UINT32 deviceIdx);
		~D3D11EventQuery();

		/** @copydoc EventQuery::begin */
		void begin(const SPtr<CommandBuffer>& cb = nullptr) override;

		/** @copydoc EventQuery::isReady */
		bool isReady() const override;

	private:
		ID3D11Query* mQuery = nullptr;
		ID3D11DeviceContext* mContext;
	};

	/** @} */
}}