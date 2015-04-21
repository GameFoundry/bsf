#include "BsGUISkin.h"
#include "BsGUIElementStyle.h"
#include "BsDebug.h"
#include "BsResources.h"
#include "BsGUISkinRTTI.h"

namespace BansheeEngine
{
	GUIElementStyle GUISkin::DefaultStyle;

	GUISkin::GUISkin()
		:Resource(false)
	{ }

	GUISkin::GUISkin(const GUISkin& skin)
		:Resource(false)
	{ }

	const GUIElementStyle* GUISkin::getStyle(const String& guiElemType) const
	{
		auto iterFind = mStyles.find(guiElemType);

		if(iterFind != mStyles.end())
			return &iterFind->second;

		LOGWRN("Cannot find GUI style with name: " + guiElemType + ". Returning default style.");

		return &DefaultStyle;
	}

	void GUISkin::setStyle(const String& guiElemType, const GUIElementStyle& style)
	{
		mStyles[guiElemType] = style;
	}

	HGUISkin GUISkin::create()
	{
		GUISkinPtr newSkin = _createPtr();

		return static_resource_cast<GUISkin>(gResources()._createResourceHandle(newSkin));
	}

	GUISkinPtr GUISkin::_createPtr()
	{
		GUISkinPtr newSkin = bs_core_ptr<GUISkin, PoolAlloc>(new (bs_alloc<GUISkin, PoolAlloc>()) GUISkin());
		newSkin->_setThisPtr(newSkin);
		newSkin->initialize();

		return newSkin;
	}

	RTTITypeBase* GUISkin::getRTTIStatic()
	{
		return GUISkinRTTI::instance();
	}

	RTTITypeBase* GUISkin::getRTTI() const
	{
		return GUISkin::getRTTIStatic();
	}
}