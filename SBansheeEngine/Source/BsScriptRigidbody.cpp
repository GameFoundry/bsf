//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include "BsScriptRigidbody.h"

namespace BansheeEngine
{
	ScriptRigidbody::ScriptRigidbody(MonoObject* instance, const SPtr<Rigidbody>& body)
		:ScriptObject(instance), mRigidbody(body)
	{ }

	void ScriptRigidbody::initRuntimeData()
	{
		// TODO
	}
}