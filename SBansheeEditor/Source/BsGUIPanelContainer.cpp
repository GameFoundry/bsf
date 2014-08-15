#include "BsGUIPanelContainer.h"
#include "BsGUIArea.h"
#include "BsScriptGUIPanel.h"
#include "BsGUILayout.h"
#include "BsGUIWidget.h"
#include "BsScriptGUIArea.h"

namespace BansheeEngine
{
	GUIPanelContainer::GUIPanelContainer(const PrivatelyConstruct& dummy, const ScriptGUIPanel& guiPanel, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(layoutOptions), mGUIPanel(&guiPanel)
	{

	}

	GUIPanelContainer::~GUIPanelContainer()
	{

	}

	GUIPanelContainer* GUIPanelContainer::create(const ScriptGUIPanel& guiPanel, const GUIOptions& layoutOptions)
	{
		return bs_new<GUIPanelContainer>(PrivatelyConstruct(), guiPanel, GUILayoutOptions::create(layoutOptions));
	}

	GUIPanelContainer* GUIPanelContainer::create(const ScriptGUIPanel& guiPanel)
	{
		return bs_new<GUIPanelContainer>(PrivatelyConstruct(), guiPanel, GUILayoutOptions::create());
	}

	void GUIPanelContainer::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		const Vector<ScriptGUIArea*> areas = mGUIPanel->getAreas();

		for (auto& scriptArea : areas)
		{
			GUIArea* area = scriptArea->getInternalValue();

			if (area->x() != x || area->y() != y)
				area->setPosition(x, y);

			if (area->width() != width || area->height() != height)
				area->setSize(width, height);

			// We want to force the layout update right away otherwise it might get delayed until next frame.
			// (Since we are currently in a middle of a layout update its possible this area was already processed)
			area->_update();
		}
	}

	Vector2I GUIPanelContainer::_getOptimalSize() const
	{
		const Vector<ScriptGUIArea*> areas = mGUIPanel->getAreas();

		Vector2I optimalSize;

		for (auto& scriptArea : areas)
		{
			GUIArea* area = scriptArea->getInternalValue();
			area->_update();

			optimalSize.x = std::max(optimalSize.x, area->getLayout()._getOptimalSize().x);
			optimalSize.y = std::max(optimalSize.y, area->getLayout()._getOptimalSize().y);
		}

		return optimalSize;
	}

	const String& GUIPanelContainer::getGUITypeName()
	{
		static String typeName = "GUIAreaContainer";
		return typeName;
	}
}