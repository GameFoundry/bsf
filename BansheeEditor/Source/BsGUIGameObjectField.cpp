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
#include "BsGameObjectManager.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIGameObjectField::DEFAULT_LABEL_WIDTH = 100;
	const String GUIGameObjectField::DROP_BUTTON_STYLE = "DropButton";
	const String GUIGameObjectField::CLEAR_BUTTON_STYLE = "ObjectClearButton";

	GUIGameObjectField::GUIGameObjectField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(layoutOptions), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mInstanceId(0)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), labelStyle);
			mLayout->addElement(mLabel);
		}

		const String* curDropButtonStyle = &dropButtonStyle;
		const String* curClearButtonStyle = &clearButtonStyle;

		if(*curDropButtonStyle == StringUtil::BLANK)
			curDropButtonStyle = &DROP_BUTTON_STYLE;

		if(*curClearButtonStyle == StringUtil::BLANK)
			curClearButtonStyle = &CLEAR_BUTTON_STYLE;

		mDropButton = GUIDropButton::create((UINT32)DragAndDropType::SceneObject, GUIOptions(GUIOption::flexibleWidth()), *curDropButtonStyle);
		mClearButton = GUIButton::create(HString(L""), *curClearButtonStyle);

		mLayout->addElement(mDropButton);
		mLayout->addElement(mClearButton);

		mDropButton->onDataDropped.connect(std::bind(&GUIGameObjectField::dataDropped, this, _1));
	}

	GUIGameObjectField::~GUIGameObjectField()
	{

	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, const GUIOptions& layoutOptions, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle, 
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIOptions& layoutOptions, const String& dropButtonStyle,
		const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(), 0, nullptr, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(layoutOptions), false);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, UINT32 labelWidth, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, UINT32 labelWidth, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, labelStyle, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, 
		const String& labelStyle, const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, labelStyle, dropButtonStyle, clearButtonStyle, 
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& dropButtonStyle, const String& clearButtonStyle)
	{
		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(), 0, nullptr, dropButtonStyle, clearButtonStyle,
			GUILayoutOptions::create(), false);
	}

	HGameObject GUIGameObjectField::getValue() const
	{
		HGameObject obj;

		if(mInstanceId != 0)
			GameObjectManager::instance().tryGetObject(mInstanceId, obj);

		return obj;
	}

	void GUIGameObjectField::setValue(const HGameObject& value)
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