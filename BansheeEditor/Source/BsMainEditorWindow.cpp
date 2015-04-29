#include "BsMainEditorWindow.h"
#include "BsEditorWindowManager.h"
#include "BsDockManager.h"
#include "BsGUIMenuBar.h"
#include "BsCamera.h"
#include "BsSceneObject.h"
#include "BsRenderTexture.h"
#include "BsApplication.h"
#include "BsProfilingManager.h"
#include "BsGUIPanel.h"
#include "BsGUILayout.h"
#include "BsEditorTestSuite.h"
#include "BsTestOutput.h"
#include "BsVirtualInput.h"

// DEBUG ONLY
#include "BsTestTextSprite.h"
#include "BsShapeMeshes2D.h"
#include "BsShapeMeshes3D.h"
#include "BsRect2.h"
#include "BsProfilerOverlay.h"
#include "BsPlatform.h"

namespace BansheeEngine
{
	MainEditorWindow* MainEditorWindow::create(const RenderWindowPtr& renderWindow)
	{
		return EditorWindowManager::instance().createMain(renderWindow);
	}

	MainEditorWindow::MainEditorWindow(const RenderWindowPtr& renderWindow)
		:EditorWindowBase(renderWindow), mDockManager(nullptr), 
		mMenuBar(bs_new<GUIMenuBar>(mGUI.get(), mRenderWindow.get()))
	{
		mDockManager = DockManager::create(this);
		mGUI->getPanel()->addElement(mDockManager);

		updateAreas();

		mMenuBar->addMenuItem(L"File/New project", nullptr, 100);
		mMenuBar->addMenuItem(L"File/Open project", nullptr, 100);
		mMenuBar->addSeparator(L"File", 99);
		mMenuBar->addMenuItem(L"File/Recent projects", nullptr, 98);
		mMenuBar->addMenuItem(L"File/Recent projects/Project A", nullptr);
		mMenuBar->addMenuItem(L"File/Recent projects/Project B", nullptr);
		mMenuBar->addMenuItem(L"File/Recent projects/Project C", nullptr);
		mMenuBar->addSeparator(L"File", 97);
		mMenuBar->addMenuItem(L"File/Exit", nullptr, 96);
		mMenuBar->addMenuItem(L"Window/Scene", nullptr);

		//GameObjectHandle<TestTextSprite> textSprite = mSceneObject->addComponent<TestTextSprite>(mCamera->getViewport().get());

		//textSprite->init(sceneCamera, "Testing in a new row, does this work?", sceneRenderTarget);

		//DrawHelper2D::instance().drawQuad(sceneCamera, FRect(0.0f, 0.2f, 0.75f, 0.5f), Color::White, DebugDrawCoordType::Normalized, 250.0f);
		//DrawHelper2D::instance().drawQuad(sceneCamera, FRect(50.0f, 50.0f, 100.0f, 50.0f), Color::Blue, DebugDrawCoordType::Pixel, 250.0f);

		//DrawHelper2D::instance().drawLine_Pixel(sceneCamera, Vector2(0, 0), Vector2(20, 20), Color::Blue, DebugDrawCoordType::Pixel, 250.0f);

		//DrawHelper2D::instance().drawLine_AA(sceneCamera, Vector2(100, 10), Vector2(120, 40), 1.0f, 1.0f, Color::Blue, DebugDrawCoordType::Pixel, 250.0f);

		AABox dbgBox(Vector3(-300, -200, 1000), Vector3(300, 300, 1500));
		//DrawHelper3D::instance().drawAABox(sceneCamera, dbgBox, Color::Green, 250.0f);

		//mProfilerOverlay = mSceneObject->addComponent<ProfilerOverlay>(sceneCamera->getViewport());
		//mProfilerOverlay->show(ProfilerOverlayType::CPUSamples);

		TestSuitePtr testSuite = TestSuite::create<EditorTestSuite>();
		testSuite->run(ExceptionTestOutput());
	}

	MainEditorWindow::~MainEditorWindow()
	{
		mDockManager->closeAll();
		GUIElement::destroy(mDockManager);
		bs_delete(mMenuBar);
	}

	void MainEditorWindow::resized()
	{
		EditorWindowBase::resized();

		updateAreas();
	}

	void MainEditorWindow::updateAreas()
	{
		UINT32 widgetWidth = (UINT32)std::max(0, (INT32)getWidth() - 2);
		UINT32 widgetHeight = (UINT32)std::max(0, (INT32)getHeight() - 2);

		UINT32 menuBarHeight = 15;
		mMenuBar->setArea(1, 1, widgetWidth, menuBarHeight);

		UINT32 dockHeight = (UINT32)std::max(0, (INT32)widgetHeight - (INT32)menuBarHeight);
		mDockManager->setArea(1, menuBarHeight + 1, widgetWidth, dockHeight);
	}

	void MainEditorWindow::update()
	{
		mDockManager->update();
	}
}