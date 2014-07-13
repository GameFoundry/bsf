#pragma once

#include "BsScriptEditorPrerequisites.h"
#include "BsGUIElementContainer.h"
#include "BsScriptGUIPanel.h"

namespace BansheeEngine
{
	class BS_SCR_BED_EXPORT GUIPanelContainer : public GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const String& getGUITypeName();

		static GUIPanelContainer* create(const ScriptGUIPanel& guiPanel, const GUIOptions& layoutOptions);
		static GUIPanelContainer* create(const ScriptGUIPanel& guiPanel);

		GUIPanelContainer(const PrivatelyConstruct& dummy, const ScriptGUIPanel& guiPanel, const GUILayoutOptions& layoutOptions);

		void setPanel(const ScriptGUIPanel& guiPanel) { mGUIPanel = &guiPanel; }

		void _updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
			RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth);

		Vector2I _getOptimalSize() const;
	protected:
		virtual ~GUIPanelContainer();

		const ScriptGUIPanel* mGUIPanel;
	};
}