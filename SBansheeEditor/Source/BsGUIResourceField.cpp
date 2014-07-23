#include "BsGUIResourceField.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUILabel.h"
#include "BsGUIDropButton.h"
#include "BsGUIButton.h"
#include "BsBuiltinResources.h"
#include "BsGUIWidget.h"
#include "BsGUIMouseEvent.h"
#include "BsGUIResourceTreeView.h"
#include "BsGUIWidget.h"
#include "BsGameObjectManager.h"
#include "BsRuntimeScriptObjects.h"
#include "BsMonoClass.h"
#include "BsResources.h"
#include "BsProjectLibrary.h"
#include "BsEditorGUI.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIResourceField::DEFAULT_LABEL_WIDTH = 100;

	GUIResourceField::GUIResourceField(const PrivatelyConstruct& dummy, const String& type, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(layoutOptions, style), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mType(type)
	{
		mLayout = &addLayoutXInternal(this);

		if (withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), getSubStyleName(EditorGUI::ObjectFieldLabelStyleName));
			mLayout->addElement(mLabel);
		}

		mDropButton = GUIDropButton::create((UINT32)DragAndDropType::SceneObject, GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(EditorGUI::ObjectFieldDropBtnStyleName));
		mClearButton = GUIButton::create(HString(L""), getSubStyleName(EditorGUI::ObjectFieldClearBtnStyleName));

		mLayout->addElement(mDropButton);
		mLayout->addElement(mClearButton);

		mDropButton->onDataDropped.connect(std::bind(&GUIResourceField::dataDropped, this, _1));
	}

	GUIResourceField::~GUIResourceField()
	{

	}

	GUIResourceField* GUIResourceField::create(const String& type, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, labelContent, labelWidth, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const GUIContent& labelContent, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, GUIContent(labelText), labelWidth, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const HString& labelText, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const GUIOptions& layoutOptions, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, GUIContent(), 0, *curStyle,
			GUILayoutOptions::create(layoutOptions), false);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, labelContent, labelWidth, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const GUIContent& labelContent,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const HString& labelText, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, GUIContent(labelText), labelWidth, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const HString& labelText,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIResourceField* GUIResourceField::create(const String& type, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIResourceField>(PrivatelyConstruct(), type, GUIContent(), 0, *curStyle,
			GUILayoutOptions::create(), false);
	}

	HResource GUIResourceField::getValue() const
	{
		return Resources::instance().loadFromUUID(mUUID);
	}

	void GUIResourceField::setValue(const HResource& value)
	{
		if (value)
			setUUID(value.getUUID());
		else
			setUUID("");
	}

	void GUIResourceField::setUUID(const String& uuid)
	{ 
		mUUID = uuid;

		Path filePath;
		if (Resources::instance().getFilePathFromUUID(mUUID, filePath))
		{
			mDropButton->setContent(GUIContent(filePath.getFilename(false)));
		}
		else
			mDropButton->setContent(GUIContent(HString(L"None")));

		onValueChanged(mUUID);
	}

	void GUIResourceField::_updateLayoutInternal(INT32 x, INT32 y, UINT32 width, UINT32 height,
		RectI clipRect, UINT8 widgetDepth, UINT16 areaDepth)
	{
		mLayout->_updateLayoutInternal(x, y, width, height, clipRect, widgetDepth, areaDepth);
	}

	Vector2I GUIResourceField::_getOptimalSize() const
	{
		return mLayout->_getOptimalSize();
	}

	void GUIResourceField::dataDropped(void* data)
	{
		DraggedResources* draggedResources = reinterpret_cast<DraggedResources*>(data);
		UINT32 numResources = (UINT32)draggedResources->resourceUUIDs.size();

		if (numResources <= 0)
			return;

		for (UINT32 i = 0; i < numResources; i++)
		{
			// TODO - I need to be able to check resource type without loading it
		}
	}

	void GUIResourceField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(EditorGUI::ObjectFieldLabelStyleName));

		mDropButton->setStyle(getSubStyleName(EditorGUI::ObjectFieldDropBtnStyleName));
		mClearButton->setStyle(getSubStyleName(EditorGUI::ObjectFieldClearBtnStyleName));
	}

	const String& GUIResourceField::getGUITypeName()
	{
		static String typeName = "GUIResourceField";
		return typeName;
	}
}