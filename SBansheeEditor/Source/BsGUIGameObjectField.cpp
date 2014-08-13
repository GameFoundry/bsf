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
#include "BsRuntimeScriptObjects.h"
#include "BsMonoClass.h"
#include "BsSceneObject.h"
#include "BsManagedComponent.h"
#include "BsMonoManager.h"
#include "BsEditorGUI.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	const UINT32 GUIGameObjectField::DEFAULT_LABEL_WIDTH = 100;

	GUIGameObjectField::GUIGameObjectField(const PrivatelyConstruct& dummy, const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style, const GUILayoutOptions& layoutOptions, bool withLabel)
		:GUIElementContainer(layoutOptions, style), mLabel(nullptr), mClearButton(nullptr), mDropButton(nullptr), mInstanceId(0), mType(type), mNamespace(typeNamespace)
	{
		mLayout = &addLayoutXInternal(this);

		if(withLabel)
		{
			mLabel = GUILabel::create(labelContent, GUIOptions(GUIOption::fixedWidth(labelWidth)), getSubStyleName(EditorGUI::ObjectFieldLabelStyleName));
			mLayout->addElement(mLabel);
		}

		mDropButton = GUIDropButton::create((UINT32)DragAndDropType::SceneObject, GUIOptions(GUIOption::flexibleWidth()), getSubStyleName(EditorGUI::ObjectFieldDropBtnStyleName));
		mClearButton = GUIButton::create(HString(L""), getSubStyleName(EditorGUI::ObjectFieldClearBtnStyleName));
		mClearButton->onClick.connect(std::bind(&GUIGameObjectField::onClearButtonClicked, this));

		mLayout->addElement(mDropButton);
		mLayout->addElement(mClearButton);

		mDropButton->onDataDropped.connect(std::bind(&GUIGameObjectField::dataDropped, this, _1));
	}

	GUIGameObjectField::~GUIGameObjectField()
	{

	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, labelContent, labelWidth, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const GUIContent& labelContent, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const HString& labelText, UINT32 labelWidth, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, GUIContent(labelText), labelWidth, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const HString& labelText, const GUIOptions& layoutOptions,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(layoutOptions), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const GUIOptions& layoutOptions, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, GUIContent(), 0, *curStyle,
			GUILayoutOptions::create(layoutOptions), false);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const GUIContent& labelContent, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, labelContent, labelWidth, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const GUIContent& labelContent,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, labelContent, DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const HString& labelText, UINT32 labelWidth,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, GUIContent(labelText), labelWidth, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const HString& labelText,
		const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, GUIContent(labelText), DEFAULT_LABEL_WIDTH, *curStyle,
			GUILayoutOptions::create(), true);
	}

	GUIGameObjectField* GUIGameObjectField::create(const String& typeNamespace, const String& type, const String& style)
	{
		const String* curStyle = &style;
		if (*curStyle == StringUtil::BLANK)
			curStyle = &EditorGUI::ObjectFieldStyleName;

		return bs_new<GUIGameObjectField>(PrivatelyConstruct(), typeNamespace, type, GUIContent(), 0, *curStyle,
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
			mDropButton->setContent(GUIContent(HString(toWString(value->getName()) + L" (" + toWString(mType) + L")")));
		}
		else
		{
			mInstanceId = 0;
			mDropButton->setContent(GUIContent(HString(L"None (" + toWString(mType) + L")")));
		}

		onValueChanged(value);
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

		if (draggedSceneObjects->numObjects <= 0)
			return;

		MonoClass* sceneObjectClass = RuntimeScriptObjects::instance().getSceneObjectClass();

		if (mType == sceneObjectClass->getFullName()) // A scene object
		{
			setValue(draggedSceneObjects->objects[0]);
		}
		else // A component
		{
			for (UINT32 i = 0; i < draggedSceneObjects->numObjects; i++)
			{
				HSceneObject so = draggedSceneObjects->objects[i];

				const Vector<HComponent>& components = so->getComponents();
				for (auto& component : components)
				{
					if (component->getTypeId() == TID_ManagedComponent) // We only care about managed components
					{
						HManagedComponent managedComponent = static_object_cast<ManagedComponent>(component);

						MonoClass* acceptedClass = MonoManager::instance().findClass(mNamespace, mType);
						MonoClass* providedClass = MonoManager::instance().findClass(managedComponent->getManagedNamespace(), managedComponent->getManagedTypeName());

						if (acceptedClass != nullptr && providedClass != nullptr)
						{
							if (providedClass->isSubClassOf(acceptedClass))
							{
								setValue(managedComponent);
							}
						}
					}
				}
			}
		}
	}

	void GUIGameObjectField::styleUpdated()
	{
		if (mLabel != nullptr)
			mLabel->setStyle(getSubStyleName(EditorGUI::ObjectFieldLabelStyleName));

		mDropButton->setStyle(getSubStyleName(EditorGUI::ObjectFieldDropBtnStyleName));
		mClearButton->setStyle(getSubStyleName(EditorGUI::ObjectFieldClearBtnStyleName));
	}

	void GUIGameObjectField::onClearButtonClicked()
	{
		setValue(HGameObject());
	}

	const String& GUIGameObjectField::getGUITypeName()
	{
		static String typeName = "GUIGameObjectField";
		return typeName;
	}
}