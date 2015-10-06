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

	bool GUISkin::hasStyle(const String& name) const
	{
		auto iterFind = mStyles.find(name);

		if (iterFind != mStyles.end())
			return true;

		return false;
	}

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

	void GUISkin::removeStyle(const String& guiElemType)
	{
		mStyles.erase(guiElemType);
	}

	Vector<String> GUISkin::getStyleNames() const
	{
		Vector<String> output;
		for (auto& pair : mStyles)
			output.push_back(pair.first);

		return output;
	}

	HGUISkin GUISkin::create()
	{
		GUISkinPtr newSkin = _createPtr();

		return static_resource_cast<GUISkin>(gResources()._createResourceHandle(newSkin));
	}

	GUISkinPtr GUISkin::_createPtr()
	{
		GUISkinPtr newSkin = bs_core_ptr<GUISkin>(new (bs_alloc<GUISkin>()) GUISkin());
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