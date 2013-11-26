#include "CmD3D9TimerQuery.h"

namespace CamelotFramework
{
	D3D9TimerQuery::D3D9TimerQuery()
		:mInitialized(false)
	{

	}

	D3D9TimerQuery::~D3D9TimerQuery()
	{
		if(mInitialized)
		{
			// TODO
		}
	}

	void D3D9TimerQuery::begin()
	{
		// TODO

		mInitialized = true;
	}

	void D3D9TimerQuery::end()
	{
		// TODO
	}

	bool D3D9TimerQuery::isReady() const
	{
		// TODO
		return false;
	}

	UINT64 D3D9TimerQuery::getTimeMs() const
	{
		// TODO
		return 0;
	}
}