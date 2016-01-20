//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsD3D11Prerequisites.h"
#include "BsEventQuery.h"

namespace BansheeEngine
{
	/**
	 * @copydoc EventQuery
	 */
	class BS_D3D11_EXPORT D3D11EventQuery : public EventQuery
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
	};
}