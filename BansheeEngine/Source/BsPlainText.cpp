//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsPlainText.h"
#include "BsResources.h"
#include "BsPlainTextRTTI.h"

namespace BansheeEngine
{
	PlainText::PlainText(const WString& data)
		:Resource(false), mString(data)
	{

	}

	HPlainText PlainText::create(const WString& data)
	{
		return static_resource_cast<PlainText>(gResources()._createResourceHandle(_createPtr(data)));
	}

	PlainTextPtr PlainText::_createPtr(const WString& data)
	{
		PlainTextPtr plainTextPtr = bs_core_ptr<PlainText>(
			new (bs_alloc<PlainText>()) PlainText(data));
		plainTextPtr->_setThisPtr(plainTextPtr);
		plainTextPtr->initialize();

		return plainTextPtr;
	}

	RTTITypeBase* PlainText::getRTTIStatic()
	{
		return PlainTextRTTI::instance();
	}

	RTTITypeBase* PlainText::getRTTI() const
	{
		return PlainText::getRTTIStatic();
	}
}