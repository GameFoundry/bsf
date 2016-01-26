//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsGUISkin.h"

namespace BansheeEngine
{
	class BS_EXPORT GUISkinEntry : public IReflectable
	{
	public:
		GUISkinEntry() { }
		GUISkinEntry(const String& name, const GUIElementStyle& style);

		String name;
		GUIElementStyle style;

	public:
		friend class GUISkinEntryRTTI;
		static RTTITypeBase* getRTTIStatic();
		virtual RTTITypeBase* getRTTI() const;
	};

	class BS_EXPORT GUISkinEntryRTTI : public RTTIType <GUISkinEntry, IReflectable, GUISkinEntryRTTI>
	{
	private:
		String& getName(GUISkinEntry* obj) { return obj->name; }
		void setName(GUISkinEntry* obj, String& val) { obj->name = val; }

		GUIElementStyle& getStyle(GUISkinEntry* obj) { return obj->style; }
		void setStyle(GUISkinEntry* obj, GUIElementStyle& val) { obj->style = val; }

	public:
		GUISkinEntryRTTI()
		{
			addPlainField("name", 0, &GUISkinEntryRTTI::getName, &GUISkinEntryRTTI::setName);
			addReflectableField("style", 1, &GUISkinEntryRTTI::getStyle, &GUISkinEntryRTTI::setStyle);
		}

		virtual const String& getRTTIName()
		{
			static String name = "GUISkinEntry";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GUISkinEntry;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return bs_shared_ptr_new<GUISkinEntry>();
		}
	};

	class BS_EXPORT GUISkinRTTI : public RTTIType <GUISkin, Resource, GUISkinRTTI>
	{
	private:
		GUISkinEntry& getStyle(GUISkin* obj, UINT32 idx) 
		{ 
			Vector<GUISkinEntry>& entries = any_cast_ref<Vector<GUISkinEntry>>(obj->mRTTIData);
			
			return entries[idx];
		}

		void setStyle(GUISkin* obj, UINT32 idx, GUISkinEntry& val) { obj->mStyles[val.name] = val.style; }
		UINT32 getStyleArraySize(GUISkin* obj) { return (UINT32)obj->mStyles.size(); }
		void setStyleArraySize(GUISkin* obj, UINT32 size) { /* Do nothing */ }

	public:
		GUISkinRTTI()
		{
			addReflectableArrayField("mStyles", 0, &GUISkinRTTI::getStyle, &GUISkinRTTI::getStyleArraySize,
				&GUISkinRTTI::setStyle, &GUISkinRTTI::setStyleArraySize);
		}

		void onSerializationStarted(IReflectable* obj)
		{
			GUISkin* skin = static_cast<GUISkin*>(obj);
			Vector<GUISkinEntry> entries;

			for (auto& style : skin->mStyles)
			{
				entries.push_back(GUISkinEntry(style.first, style.second));
			}

			skin->mRTTIData = entries;
		}

		void onSerializationEnded(IReflectable* obj)
		{
			GUISkin* skin = static_cast<GUISkin*>(obj);

			skin->mRTTIData = nullptr;
		}

		virtual const String& getRTTIName()
		{
			static String name = "GUISkin";
			return name;
		}

		virtual UINT32 getRTTIId()
		{
			return TID_GUISkin;
		}

		virtual std::shared_ptr<IReflectable> newRTTIObject()
		{
			return GUISkin::_createPtr();
		}
	};
}