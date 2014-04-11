#include "BsGUIGameObjectField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIDropButton.h"
#include "BsGUIButton.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIWidget.h"
#include "CmGameObjectManager.h"

using namespace CamelotFramework;
using namespace BansheeEngine;
using namespace std::placeholders;

namespace BansheeEditor
{
	const UINT32 GUIGameObjectField::DEFAULT_LABEL_WIDTH = 100;

	GUIGameObjectField::GUIGameObjectField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, CM::UINT32 labelWidth,
		GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mInstanceId(0)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			const GUIElementStyle* curLabelStyle = labelStyle;

			if(curLabelStyle == nullptr)
				curLabelStyle = parent.getSkin().getStyle("Label");

			mLabel = GUILabel::create(parent, labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), curLabelStyle);
			mLayout->addElement(mLabel);
		}

		const GUIElementStyle* curDropButtonStyle = dropButtonStyle;
		const GUIElementStyle* curClearButtonStyle = clearButtonStyle;

		if(curDropButtonStyle == nullptr)
			curDropButtonStyle = parent.getSkin().getStyle("DropButton");

		if(curClearButtonStyle == nullptr)
			curClearButtonStyle = parent.getSkin().getStyle("ObjectClearButton");

		mDropButton = GUIDropButton::create(parent, (UINT32)DragAndDropType::SceneObject, GUIOptions(GUIOption::flexibleWidth()), curDropButtonStyle);
		mClearButton = GUIButton::create(parent, HString(L""), curClearButtonStyle);

		mLayout->addElement(mDropButton);
		mLayout->addElement(mClearButton);

		mDropButton->onDataDropped.connect(std::bind(&GUIGameObjectField::dataDropped, this, _1));
	}

	GUIGameObjectField::~GUIGameObjectField()
	{

	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelContent, labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, const BS::GUIOptions& layoutOptions, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelContent, DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const CM::HString& labelText, CM::UINT32 labelWidth, const BS::GUIOptions& layoutOptions, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, BS::GUIContent(labelText), labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const CM::HString& labelText, const BS::GUIOptions& layoutOptions, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, BS::GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const BS::GUIOptions& layoutOptions, BS::GUIElementStyle* dropButtonStyle,
		BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, BS::GUIContent(), 0, nullptr, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle), false);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, CM::UINT32 labelWidth, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelContent, labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const BS::GUIContent& labelContent, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelContent, DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const CM::HString& labelText, CM::UINT32 labelWidth, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, BS::GUIContent(labelText), labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, const CM::HString& labelText, 
		BS::GUIElementStyle* labelStyle, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, BS::GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(BS::GUIWidget& parent, BS::GUIElementStyle* dropButtonStyle, BS::GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, BS::GUIContent(), 0, nullptr, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(&GUISkin::DefaultStyle), false);
	}

	CM::HGameObject GUIGameObjectField::getValue() const
	{
		HGameObject obj;

		if(mInstanceId != 0)
			GameObjectManager::instance().tryGetObject(mInstanceId, obj);

		return obj;
	}

	void GUIGameObjectField::setValue(const CM::HGameObject& value)
	{
		if(value)
		{
			mInstanceId = value->getInstanceId();
			mDropButton->setContent(GUIContent(HString(toWString(value->getName()))));
		}
		else
		{
			mInstanceId = 0;
			mDropButton->setContent(GUIContent(HString(L"None")));
		}		
	}

	void GUIGameObjectField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		mLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
	}

	Vector2I GUIGameObjectField::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
	}

	void GUIGameObjectField::dataDropped(void* data)
	{
		DraggedSceneObjects* draggedSceneObjects = reinterpret_cast<DraggedSceneObjects*>(data);

		// TODO
	}

	const String& GUIGameObjectField::getGUITypeName()
	{
		static String typeName = "GUIGameObjectField";
		return typeName;
	}
}