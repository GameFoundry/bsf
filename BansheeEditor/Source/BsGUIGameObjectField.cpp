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

	GUIGameObjectField::GUIGameObjectField(const PrivatelyConstruct& dummy, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(layoutOptions, style), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mInstanceId(0)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), getSubStyleName(getLabelStyleType()));
			mLayout->addElement(mLabel);
		}

		mDropButton = GUIDropButton::create((UINT32)DragAndDropType::SceneObject, GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(getDropButtonStyleType()));
		mClearButton = GUIButton::create(HString(L""), getSubStyleName(getClearButtonStyleType()));

		mLayout->addElement(mDropButton);
		mLayout->addElement(mClearButton);

		mDropButton->onDataDropped.connect(std::bind(&GUIGameObjectField::dataDropped, this, _1));
	}

	GUIGameObjectField::~GUIGameObjectField()
	{

	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, labelWidth, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, const GUIOptions& layoutOptions, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, const GUIOptions& layoutOptions, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIOptions& layoutOptions, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(), 0, *curStyle,
			GUILayoutOptions::create(layoutOptions), false);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, UINT32 labelWidth, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, labelWidth, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const GUIContent& labelContent, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, UINT32 labelWidth, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), labelWidth, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const HString& labelText, 
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &getGUITypeName();

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), GUIContent(), 0, *curStyle,
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

	void GUIGameObjectField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(getLabelStyleType()));

		mDropButton->setStyle(getSubStyleName(getDropButtonStyleType()));
		mClearButton->setStyle(getSubStyleName(getClearButtonStyleType()));
	}

	const String& GUIGameObjectField::getGUITypeName()
	{
		static String typeName = "GUIGameObjectField";
		return typeName;
	}

	const String& GUIGameObjectField::getLabelStyleType()
	{
		static String STYLE_TYPE = "EditorFieldLabel";
		return STYLE_TYPE;
	}

	const String& GUIGameObjectField::getDropButtonStyleType()
	{
		static String typeName = "DropButton";
		return typeName;
	}

	const String& GUIGameObjectField::getClearButtonStyleType()
	{
		static String typeName = "ObjectClearButton";
		return typeName;
	}
}