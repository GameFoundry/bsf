#include "CmTestTextSprite.h"
#include "CmSceneObject.h"
#include "BsRenderable.h"
#include "CmMesh.h"
#include "CmVector2.h"
#include "BsTextSprite.h"
#include "CmFont.h"
#include "CmMaterial.h"
#include "BsGUILabel.h"
#include "BsGUIListBox.h"
#include "BsGUISkin.h"
#include "BsOverlayManager.h"
#include "BsSpriteTexture.h"
#include "BsEditorGUI.h"
#include "BsGUITexture.h"
#include "BsGUIRenderTexture.h"
#include "BsGUIArea.h"
#include "BsGUILayout.h"
#include "BsGUISpace.h"
#include "BsGUIViewport.h"
#include "BsGUIButton.h"
#include "BsCamera.h"
#include "CmInput.h"
#include "CmPlatform.h"
#include "BsGUIContent.h"
#include "CmStringTable.h"
#include "BsGUISceneTreeView.h"
#include "BsGUIResourceTreeView.h"
#include "BsGUIScrollArea.h"
#include "BsGUIIntField.h"
#include "BsGUIFloatField.h"

using namespace CamelotFramework;
using namespace BansheeEngine;

namespace BansheeEditor
{
	TestTextSprite::TestTextSprite(const HSceneObject& parent, CM::Viewport* target)
		:GUIWidget(parent, target), mListBox(nullptr)
	{
	}

	TestTextSprite::~TestTextSprite()
	{
	}

	void TestTextSprite::init(const HCamera& camera, const String& text, CM::RenderTexturePtr sceneView)
	{
		setSkin(BansheeEditor::EditorGUI::instance().getSkin());
		setDepth(128);

		GUIArea* area = GUIArea::createStretchedXY(*this, 0, 0, 0, 0);

		mSceneTreeView = GUISceneTreeView::create(*this, GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()));
		mResourceTreeView = GUIResourceTreeView::create(*this, GUIOptions(GUIOption::flexibleWidth(), GUIOption::flexibleHeight()));
		GUILayout& sceneTreeViewLayout = area->getLayout().addLayoutY();
		
		//sceneTreeViewLayout.addElement(mSceneTreeView);
		

		SceneObject::create("FILLER_A");
		SceneObject::create("FILLER_B");
		SceneObject::create("FILLER_C");
		SceneObject::create("FILLER_D");
		SceneObject::create("FILLER_E");
		SceneObject::create("FILLER_F");
		SceneObject::create("FILLER_G");
		SceneObject::create("FILLER_H");
		SceneObject::create("FILLER_I");
		SceneObject::create("FILLER_J");
		SceneObject::create("FILLER_K");
		SceneObject::create("FILLER_L");

		area->getLayout().addElement(GUIRenderTexture::create(*this, sceneView, GUIOptions(GUIOption::fixedWidth(800), GUIOption::fixedHeight(600))));
		//mLabel = GUILabel::create(*this, HString(L""));
		//area->getLayout().addElement(mLabel);

		//Vector<HString>::type dropDownElements;
		//dropDownElements.push_back(HString(L"Ejlement #1"));
		//dropDownElements.push_back(HString(L"Element #2"));
		//dropDownElements.push_back(HString(L"Element #3"));
		//mListBox = GUIListBox::create(*this, dropDownElements, GUIOptions(GUIOption::fixedWidth(50), GUIOption::fixedHeight(13)));
		//area->getLayout().addElement(mListBox);

		GUIScrollArea* scrollArea = GUIScrollArea::create(*this, GUIOptions(GUIOption::fixedWidth(100), GUIOption::fixedHeight(100)));
		sceneTreeViewLayout.addFlexibleSpace();
		area->getLayout().addElement(scrollArea);
		sceneTreeViewLayout.addFlexibleSpace();

		scrollArea->getLayout().addElement(mSceneTreeView);
		area->getLayout().addElement(mResourceTreeView);

		GUIIntField* intField = GUIIntField::create(*this, HString(L"Int Field"), GUIOptions(GUIOption::fixedWidth(200)));
		GUIFloatField* floatField = GUIFloatField::create(*this, HString(L"Float Field"), GUIOptions(GUIOption::fixedWidth(200)));
		area->getLayout().addElement(intField);
		area->getLayout().addElement(floatField);
		//GUIButton* button = GUIButton::create(*this, HString(L"dbgBtn"));
		//button->onClick.connect(boost::bind(&TestTextSprite::dbgBtn, this));
		//area->getLayout().addElement(button);

		//button = GUIButton::create(*this, HString(L"Add GameObjects"));
		//button->onClick.connect(boost::bind(&TestTextSprite::dbgAdd, this));
		//area->getLayout().addElement(button);

		//button = GUIButton::create(*this, HString(L"Rename GameObject"));
		//button->onClick.connect(boost::bind(&TestTextSprite::dbgRename, this));
		//area->getLayout().addElement(button);

		//button = GUIButton::create(*this, HString(L"Remove child GameObjects"));
		//button->onClick.connect(boost::bind(&TestTextSprite::dbgRemoveChildren, this));
		//area->getLayout().addElement(button);

		//button = GUIButton::create(*this, HString(L"Remove parent GameObjects"));
		//button->onClick.connect(boost::bind(&TestTextSprite::dbgRemoveParents, this));
		//area->getLayout().addElement(button);

		area->getLayout().addFlexibleSpace();

		labelString = HString(L"\\{0}, {1}");
		//mLabel->setContent(GUIContent(labelString));
	}

	void TestTextSprite::update()
	{
		labelString.setParameter(0, toWString(Input::instance().getCursorPosition().x));
		mSceneTreeView->update();
		mResourceTreeView->update();
		//labelString.setParameter(1, toWString(Input::instance().getCursorPosition().y));
	}

	void TestTextSprite::dbgBtn()
	{
		static int dbg = 0;

		if(dbg == 0)
		{
			//Vector<HString>::type dropDownElements;
			//dropDownElements.push_back(HString(L"Element #4"));
			//dropDownElements.push_back(HString(L"Element #5"));
			//dropDownElements.push_back(HString(L"Element #6"));

			//mListBox->setElements(dropDownElements);

			//StringTable::instance().setString(L"dbgBtn", Language::Abkhazian, L"ALOALO");
			//StringTable::instance().setActiveLanguage(Language::Abkhazian);
		}
		else if(dbg == 1)
		{
			//StringTable::instance().removeString(L"dbgBtn");
		}

		dbg++;
	}

	void TestTextSprite::dbgAdd()
	{
		mDbgMainA = SceneObject::create("DEBUG_A");
		mDbgChildA = SceneObject::create("DEBUG_CHILD_0");
		mDbgChildA->setParent(mDbgMainA);

		mDbgChildB = SceneObject::create("DEBUG_CHILD_1");
		mDbgChildB->setParent(mDbgMainA);

		mDbgChildC = SceneObject::create("DEBUG_CHILD_2");
		mDbgChildC->setParent(mDbgMainA);

		mDbgMainB = SceneObject::create("DEBUG_B");
	}

	void TestTextSprite::dbgRename()
	{
		mDbgMainA->setName("Z_DEBUG_RENAMED_A");
	}

	void TestTextSprite::dbgRemoveChildren()
	{
		mDbgChildA->destroy();
		mDbgChildB->destroy();
	}

	void TestTextSprite::dbgRemoveParents()
	{
		mDbgMainA->destroy();
		mDbgMainB->destroy();
	}
}