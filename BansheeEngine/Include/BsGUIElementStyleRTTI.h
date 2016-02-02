//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsPrerequisites.h"
#include "BsRTTIType.h"
#include "BsGUIElementStyle.h"

namespace BansheeEngine
{
	/** @cond INTERNAL */
	/** @addtogroup RTTI-Impl-Engine
	 *  @{
	 */

	class BS_EXPORT GUIElementStyleRTTI : public RTTIType <GUIElementStyle, IReflectable, GUIElementStyleRTTI>
	{
	private:
		HFont& getFont(GUIElementStyle* obj) { return obj->font; }
		void setFont(GUIElementStyle* obj, HFont& val) { obj->font = val; }
		
		UINT32& getFontSize(GUIElementStyle* obj) { return obj->fontSize; }
		void setFontSize(GUIElementStyle* obj, UINT32& val) { obj->fontSize = val; }

		TextHorzAlign& getTextHorzAlign(GUIElementStyle* obj) { return obj->textHorzAlign; }
		void setTextHorzAlign(GUIElementStyle* obj, TextHorzAlign& val) { obj->textHorzAlign = val; }

		TextVertAlign& getTextVertAlign(GUIElementStyle* obj) { return obj->textVertAlign; }
		void setTextVertAlign(GUIElementStyle* obj, TextVertAlign& val) { obj->textVertAlign = val; }

		GUIImagePosition& getImagePosition(GUIElementStyle* obj) { return obj->imagePosition; }
		void setImagePosition(GUIElementStyle* obj, GUIImagePosition& val) { obj->imagePosition = val; }

		bool& getWordWrap(GUIElementStyle* obj) { return obj->wordWrap; }
		void setWordWrap(GUIElementStyle* obj, bool& val) { obj->wordWrap = val; }


		HSpriteTexture& getNormalTexture(GUIElementStyle* obj) { return obj->normal.texture; }
		void setNormalTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->normal.texture = val; }

		Color& getNormalTextColor(GUIElementStyle* obj) { return obj->normal.textColor; }
		void setNormalTextColor(GUIElementStyle* obj, Color& val) { obj->normal.textColor = val; }

		HSpriteTexture& getHoverTexture(GUIElementStyle* obj) { return obj->hover.texture; }
		void setHoverTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->hover.texture = val; }

		Color& getHoverTextColor(GUIElementStyle* obj) { return obj->hover.textColor; }
		void setHoverTextColor(GUIElementStyle* obj, Color& val) { obj->hover.textColor = val; }

		HSpriteTexture& getActiveTexture(GUIElementStyle* obj) { return obj->active.texture; }
		void setActiveTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->active.texture = val; }

		Color& getActiveTextColor(GUIElementStyle* obj) { return obj->active.textColor; }
		void setActiveTextColor(GUIElementStyle* obj, Color& val) { obj->active.textColor = val; }

		HSpriteTexture& getFocusedTexture(GUIElementStyle* obj) { return obj->focused.texture; }
		void setFocusedTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->focused.texture = val; }

		Color& getFocusedTextColor(GUIElementStyle* obj) { return obj->focused.textColor; }
		void setFocusedTextColor(GUIElementStyle* obj, Color& val) { obj->focused.textColor = val; }


		HSpriteTexture& getNormalOnTexture(GUIElementStyle* obj) { return obj->normalOn.texture; }
		void setNormalOnTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->normalOn.texture = val; }

		Color& getNormalOnTextColor(GUIElementStyle* obj) { return obj->normalOn.textColor; }
		void setNormalOnTextColor(GUIElementStyle* obj, Color& val) { obj->normalOn.textColor = val; }

		HSpriteTexture& getHoverOnTexture(GUIElementStyle* obj) { return obj->hoverOn.texture; }
		void setHoverOnTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->hoverOn.texture = val; }

		Color& getHoverOnTextColor(GUIElementStyle* obj) { return obj->hoverOn.textColor; }
		void setHoverOnTextColor(GUIElementStyle* obj, Color& val) { obj->hoverOn.textColor = val; }

		HSpriteTexture& getActiveOnTexture(GUIElementStyle* obj) { return obj->activeOn.texture; }
		void setActiveOnTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->activeOn.texture = val; }

		Color& getActiveOnTextColor(GUIElementStyle* obj) { return obj->activeOn.textColor; }
		void setActiveOnTextColor(GUIElementStyle* obj, Color& val) { obj->activeOn.textColor = val; }

		HSpriteTexture& getFocusedOnTexture(GUIElementStyle* obj) { return obj->focusedOn.texture; }
		void setFocusedOnTexture(GUIElementStyle* obj, HSpriteTexture& val) { obj->focusedOn.texture = val; }

		Color& getFocusedOnTextColor(GUIElementStyle* obj) { return obj->focusedOn.textColor; }
		void setFocusedOnTextColor(GUIElementStyle* obj, Color& val) { obj->focusedOn.textColor = val; }


		RectOffset& getBorder(GUIElementStyle* obj) { return obj->border; }
		void setBorder(GUIElementStyle* obj, RectOffset& val) { obj->border = val; }

		RectOffset& getMargins(GUIElementStyle* obj) { return obj->margins; }
		void setMargins(GUIElementStyle* obj, RectOffset& val) { obj->margins = val; }

		RectOffset& getContentOffset(GUIElementStyle* obj) { return obj->contentOffset; }
		void setContentOffset(GUIElementStyle* obj, RectOffset& val) { obj->contentOffset = val; }

		RectOffset& getPadding(GUIElementStyle* obj) { return obj->padding; }
		void setPadding(GUIElementStyle* obj, RectOffset& val) { obj->padding = val; }

		UINT32& getWidth(GUIElementStyle* obj) { return obj->width; }
		void setWidth(GUIElementStyle* obj, UINT32& val) { obj->width = val; }

		UINT32& getHeight(GUIElementStyle* obj) { return obj->height; }
		void setHeight(GUIElementStyle* obj, UINT32& val) { obj->height = val; }

		UINT32& getMinWidth(GUIElementStyle* obj) { return obj->minWidth; }
		void setMinWidth(GUIElementStyle* obj, UINT32& val) { obj->minWidth = val; }

		UINT32& getMaxWidth(GUIElementStyle* obj) { return obj->maxWidth; }
		void setMaxWidth(GUIElementStyle* obj, UINT32& val) { obj->maxWidth = val; }

		UINT32& getMinHeight(GUIElementStyle* obj) { return obj->minHeight; }
		void setMinHeight(GUIElementStyle* obj, UINT32& val) { obj->minHeight = val; }

		UINT32& getMaxHeight(GUIElementStyle* obj) { return obj->maxHeight; }
		void setMaxHeight(GUIElementStyle* obj, UINT32& val) { obj->maxHeight = val; }

		bool& getFixedWidth(GUIElementStyle* obj) { return obj->fixedWidth; }
		void setFixedWidth(GUIElementStyle* obj, bool& val) { obj->fixedWidth = val; }

		bool& getFixedHeight(GUIElementStyle* obj) { return obj->fixedHeight; }
		void setFixedHeight(GUIElementStyle* obj, bool& val) { obj->fixedHeight = val; }

		Map<String, String>& getSubStyles(GUIElementStyle* obj) { return obj->subStyles; }
		void setSubStyles(GUIElementStyle* obj, Map<String, String>& val) { obj->subStyles = val; }

	public:
		GUIElementStyleRTTI()
		{
			addReflectableField("font", 0, &GUIElementStyleRTTI::getFont, &GUIElementStyleRTTI::setFont);
			addPlainField("fontSize", 1, &GUIElementStyleRTTI::getFontSize, &GUIElementStyleRTTI::setFontSize);
			addPlainField("textHorzAlign", 2, &GUIElementStyleRTTI::getTextHorzAlign, &GUIElementStyleRTTI::setTextHorzAlign);
			addPlainField("textVertAlign", 3, &GUIElementStyleRTTI::getTextVertAlign, &GUIElementStyleRTTI::setTextVertAlign);
			addPlainField("imagePosition", 4, &GUIElementStyleRTTI::getImagePosition, &GUIElementStyleRTTI::setImagePosition);
			addPlainField("wordWrap", 5, &GUIElementStyleRTTI::getWordWrap, &GUIElementStyleRTTI::setWordWrap);

			addReflectableField("normalTex", 6, &GUIElementStyleRTTI::getNormalTexture, &GUIElementStyleRTTI::setNormalTexture);
			addPlainField("normalTextColor", 7, &GUIElementStyleRTTI::getNormalTextColor, &GUIElementStyleRTTI::setNormalTextColor);
			addReflectableField("hoverTex", 8, &GUIElementStyleRTTI::getHoverTexture, &GUIElementStyleRTTI::setHoverTexture);
			addPlainField("hoverTextColor", 9, &GUIElementStyleRTTI::getHoverTextColor, &GUIElementStyleRTTI::setHoverTextColor);
			addReflectableField("activeTex", 10, &GUIElementStyleRTTI::getActiveTexture, &GUIElementStyleRTTI::setActiveTexture);
			addPlainField("activeTextColor", 11, &GUIElementStyleRTTI::getActiveTextColor, &GUIElementStyleRTTI::setActiveTextColor);
			addReflectableField("focusedTex", 12, &GUIElementStyleRTTI::getFocusedTexture, &GUIElementStyleRTTI::setFocusedTexture);
			addPlainField("focusedTextColor", 13, &GUIElementStyleRTTI::getFocusedTextColor, &GUIElementStyleRTTI::setFocusedTextColor);

			addReflectableField("normalOnTex", 14, &GUIElementStyleRTTI::getNormalOnTexture, &GUIElementStyleRTTI::setNormalOnTexture);
			addPlainField("normalOnTextColor", 15, &GUIElementStyleRTTI::getNormalOnTextColor, &GUIElementStyleRTTI::setNormalOnTextColor);
			addReflectableField("hoverOnTex", 16, &GUIElementStyleRTTI::getHoverOnTexture, &GUIElementStyleRTTI::setHoverOnTexture);
			addPlainField("hoverOnTextColor", 17, &GUIElementStyleRTTI::getHoverOnTextColor, &GUIElementStyleRTTI::setHoverOnTextColor);
			addReflectableField("activeOnTex", 18, &GUIElementStyleRTTI::getActiveOnTexture, &GUIElementStyleRTTI::setActiveOnTexture);
			addPlainField("activeOnTextColor", 19, &GUIElementStyleRTTI::getActiveOnTextColor, &GUIElementStyleRTTI::setActiveOnTextColor);
			addReflectableField("focusedOnTex", 20, &GUIElementStyleRTTI::getFocusedOnTexture, &GUIElementStyleRTTI::setFocusedOnTexture);
			addPlainField("focusedOnTextColor", 21, &GUIElementStyleRTTI::getFocusedOnTextColor, &GUIElementStyleRTTI::setFocusedOnTextColor);

			addPlainField("border", 22, &GUIElementStyleRTTI::getBorder, &GUIElementStyleRTTI::setBorder);
			addPlainField("margins", 23, &GUIElementStyleRTTI::getMargins, &GUIElementStyleRTTI::setMargins);
			addPlainField("contentOffset", 24, &GUIElementStyleRTTI::getContentOffset, &GUIElementStyleRTTI::setContentOffset);
			addPlainField("padding", 25, &GUIElementStyleRTTI::getPadding, &GUIElementStyleRTTI::setPadding);

			addPlainField("width", 26, &GUIElementStyleRTTI::getWidth, &GUIElementStyleRTTI::setWidth);
			addPlainField("height", 27, &GUIElementStyleRTTI::getHeight, &GUIElementStyleRTTI::setHeight);
			addPlainField("minWidth", 28, &GUIElementStyleRTTI::getMinWidth, &GUIElementStyleRTTI::setMinWidth);
			addPlainField("maxWidth", 29, &GUIElementStyleRTTI::getMaxWidth, &GUIElementStyleRTTI::setMaxWidth);
			addPlainField("minHeight", 30, &GUIElementStyleRTTI::getMinHeight, &GUIElementStyleRTTI::setMinHeight);
			addPlainField("maxHeight", 31, &GUIElementStyleRTTI::getMaxHeight, &GUIElementStyleRTTI::setMaxHeight);
			addPlainField("fixedWidth", 32, &GUIElementStyleRTTI::getFixedWidth, &GUIElementStyleRTTI::setFixedWidth);
			addPlainField("fixedHeight", 33, &GUIElementStyleRTTI::getFixedHeight, &GUIElementStyleRTTI::setFixedHeight);

			addPlainField("subStyles", 34, &GUIElementStyleRTTI::getSubStyles, &GUIElementStyleRTTI::setSubStyles);
		}

		const String& getRTTIName() override
		{
			static String name = "GUIElementStyle";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_GUIElementStyle;
		}

		std::shared_ptr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<GUIElementStyle>();
		}
	};

	/** @} */
	/** @endcond */
}