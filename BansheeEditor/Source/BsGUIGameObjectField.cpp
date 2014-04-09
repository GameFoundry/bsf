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

namespace BansheeEditor
{
	GUIGameObjectField::GUIGameObjectField(const PrivatelyConstruct& dummy, GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mLabelWidth(100), mInstanceId(0)
	{
		construct(parent, labelContent, labelStyle, dropButtonStyle, clearButtonStyle, layoutOptions, true);
	}

	GUIGameObjectField::GUIGameObjectField(const PrivatelyConstruct& dummy, GUIWidget& parent, 
		GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle, const GUILayoutOptions& layoutOptions)
		:GUIElementContainer(parent, layoutOptions), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mLabelWidth(100), mInstanceId(0)
	{
		construct(parent, GUIContent(), labelStyle, dropButtonStyle, clearButtonStyle, layoutOptions, false);
	}

	GUIGameObjectField::~GUIGameObjectField()
	{

	}

	void GUIGameObjectField::construct(GUIWidget& parent, const GUIContent& labelContent, 
		GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle,
		GUIElementStyle* clearButtonStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			const GUIElementStyle* curLabelStyle = labelStyle;

			if(curLabelStyle == nullptr)
				curLabelStyle = parent.getSkin().getStyle("Label");

			mLabel = GUILabel::create(parent, labelContent, GUIOptions(GUIOption::fixedWidth(mLabelWidth)), curLabelStyle);
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

		mDropButton->onDataDropped.connect(boost::bind(&GUIGameObjectField::dataDropped, this, _1));
	}

	GUIGameObjectField* GUIGameObjectField::create(GUIWidget& parent, const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelContent, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIGameObjectField* GUIGameObjectField::create(GUIWidget& parent, const GUIContent& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelContent, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIGameObjectField* GUIGameObjectField::create(GUIWidget& parent, const HString& labelContent, const GUIOptions& layoutOptions, 
		GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, 
			dropButtonStyle, clearButtonStyle, GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIGameObjectField* GUIGameObjectField::create(GUIWidget& parent, const HString& labelContent, GUIElementStyle* labelStyle, 
		GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, GUIContent(labelContent), labelStyle, dropButtonStyle, 
			clearButtonStyle, GUILayoutOptions::create(&GUISkin::DefaultStyle));
	}

	GUIGameObjectField* GUIGameObjectField::create(GUIWidget& parent, const GUIOptions& layoutOptions, GUIElementStyle* labelStyle, 
		GUIElementStyle* dropButtonStyle, GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelStyle, dropButtonStyle, clearButtonStyle, 
			GUILayoutOptions::create(layoutOptions, &GUISkin::DefaultStyle));
	}

	GUIGameObjectField* GUIGameObjectField::create(GUIWidget& parent, GUIElementStyle* labelStyle, GUIElementStyle* dropButtonStyle, 
		GUIElementStyle* clearButtonStyle)
	{
		return cm_new<GUIGameObjectField>(PrivatelyConstruct(), parent, labelStyle, dropButtonStyle, clearButtonStyle, 
			GUILayoutOptions::create(&GUISkin::DefaultStyle));
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

	void GUIGameObjectField::setLabelWidth(UINT32 width)
	{
		mLabelWidth = width;

		if(mLabel != nullptr)
		{
			//mLabel->
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