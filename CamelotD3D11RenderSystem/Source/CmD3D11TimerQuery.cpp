#include "CmD3D11TimerQuery.h"

namespace CamelotFramework
{
	D3D11TimerQuery::D3D11TimerQuery()
		:mInitialized(false)
	{

	}

	D3D11TimerQuery::~D3D11TimerQuery()
	{
		if(mInitialized)
		{
			// TODO
		}
	}

	void D3D11TimerQuery::begin()
	{
		// TODO

		mInitialized = true;
	}

	void D3D11TimerQuery::end()
	{
		// TODO
	}

	bool D3D11TimerQuery::isReady() const
	{
		// TODO
		return false;
	}

	UINT64 D3D11TimerQuery::getTimeMs() const
	{
		// TODO
		return 0;
	}
}