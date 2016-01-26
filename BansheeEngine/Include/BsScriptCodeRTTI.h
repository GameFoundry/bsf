//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsScriptCode.h"

namespace BansheeEngine
{
	class BS_EXPORT ScriptCodeRTTI : public RTTIType <ScriptCode, Resource, ScriptCodeRTTI>
	{
	private:
		WString& getString(ScriptCode* obj) { return obj->mString; }
		void setString(ScriptCode* obj, WString& val) { obj->mString = val; }

		bool& getEditorScript(ScriptCode* obj) { return obj->mEditorScript; }
		void setEditorScript(ScriptCode* obj, bool& val) { obj->mEditorScript = val; }
	public:
		ScriptCodeRTTI()
		{
			addPlainField("mString", 0, &ScriptCodeRTTI::getString, &ScriptCodeRTTI::setString);
			addPlainField("mEditorScript", 1, &ScriptCodeRTTI::getEditorScript, &ScriptCodeRTTI::setEditorScript);
		}

		virtual const String& getRTTIName()
		{
			static String name = "ScriptCode";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_ScriptCode;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return ScriptCode::_createPtr(L""); // Initial string doesn't matter, it'll get overwritten
		}
	};
}