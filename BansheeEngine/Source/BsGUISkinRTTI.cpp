#include "BsGUISkinRTTI.h"

namespace BansheeEngine
{
	GUISkinEntry::GUISkinEntry(const String& name, const GUIElementStyle& style)
		:name(name), style(style)
	{ }

	RTTITypeBase* GUISkinEntry::getRTTIStatic()
	{
		return GUISkinEntryRTTI::instance();
	}

	RTTITypeBase* GUISkinEntry::getRTTI() const
	{
		return GUISkinEntry::getRTTIStatic();
	}
}