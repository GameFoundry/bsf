#pragma once

#include "BsEditorPrerequisites.h"
#include "BsGUIElementContainer.h"

namespace BansheeEditor
{
	class BS_ED_EXPORT GUIFoldout : public BS::GUIElementContainer
	{
		struct PrivatelyConstruct {};

	public:
		static const CM::String& getGUITypeName();

		static GUIFoldout* create(const BS::GUIOptions& layoutOptions, 
			BS::GUIElementStyle* buttonStyle = nullptr, BS::GUIElementStyle* backgroundStyle = nullptr);

		static GUIFoldout* create(BS::GUIElementStyle* labelStyle = nullptr,
			BS::GUIElementStyle* backgroundStyle = nullptr);

		GUIFoldout(const PrivatelyConstruct& dummy, BS::GUIElementStyle* buttonStyle, 
			BS::GUIElementStyle* backgroundStyle, const BS::GUILayoutOptions& layoutOptions);

		bool isExpanded() const { return mIsExpanded; }
		void setExpanded(bool expanded);

		void _updateLayoutInternal(CM::INT32 x, CM::INT32 y, CM::UINT32 width, CM::UINT32 height,
			CM::RectI clipRect, CM::UINT8 widgetDepth, CM::UINT16 areaDepth);

		CM::Vector2I _getOptimalSize() const;

		boost::signal<void(bool)> onStateChanged;
	protected:
		virtual ~GUIFoldout();

	protected:
		BS::GUIToggle* mToggle;
		BS::GUITexture* mBackground;

		bool mIsExpanded;
	};
}