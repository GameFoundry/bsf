#include "BsScriptDropTarget.h"
#include "BsScriptMeta.h"
#include "BsMonoField.h"
#include "BsMonoClass.h"
#include "BsMonoManager.h"
#include "BsMonoMethod.h"
#include "BsMonoUtil.h"
#include "BsMonoArray.h"
#include "BsRTTIType.h"
#include "BsPlatform.h"
#include "BsEditorWidget.h"
#include "BsEditorWindowBase.h"
#include "BsEditorWidgetContainer.h"
#include "BsScriptEditorWindow.h"

using namespace std::placeholders;

namespace BansheeEngine
{
	ScriptDropTarget::OnEnterThunkDef ScriptDropTarget::onEnterThunk;
	ScriptDropTarget::OnMoveDef ScriptDropTarget::onMoveThunk;
	ScriptDropTarget::OnLeaveDef ScriptDropTarget::onLeaveThunk;
	ScriptDropTarget::OnDropThunkDef ScriptDropTarget::onDropThunk;

	ScriptDropTarget::ScriptDropTarget(MonoObject* instance, EditorWidgetBase* parent)
		:ScriptObject(instance), mDropTarget(nullptr), mIsDestroyed(false)
	{
		mWidgetParentChangedConn = parent->onParentChanged.connect(std::bind(&ScriptDropTarget::widgetParentChanged, this, _1));
		mWidgetResizedConn = parent->onResized.connect(std::bind(&ScriptDropTarget::widgetResized, this, _1, _2));

		if (parent != nullptr)
		{
			EditorWindowBase* parentWindow = parent->getParentWindow();

			if (parentWindow != nullptr)
				setDropTarget(parentWindow->getRenderWindow(), 0, 0, 0, 0);
		}
	}

	ScriptDropTarget::~ScriptDropTarget()
	{
		if (!mIsDestroyed)
			destroy();
	}

	void ScriptDropTarget::initRuntimeData()
	{
		metaData.scriptClass->addInternalCall("Internal_CreateInstance", &ScriptDropTarget::internal_CreateInstance);
		metaData.scriptClass->addInternalCall("Internal_Destroy", &ScriptDropTarget::internal_Destroy);
		metaData.scriptClass->addInternalCall("Internal_SetBounds", &ScriptDropTarget::internal_SetBounds);
		metaData.scriptClass->addInternalCall("Internal_GetFilePaths", &ScriptDropTarget::internal_GetFilePaths);

		onEnterThunk = (OnEnterThunkDef)metaData.scriptClass->getMethod("InternalDoOnEnter")->getThunk();
		onMoveThunk = (OnMoveDef)metaData.scriptClass->getMethod("InternalDoOnDrag")->getThunk();
		onLeaveThunk = (OnLeaveDef)metaData.scriptClass->getMethod("InternalDoOnLeave")->getThunk();
		onDropThunk = (OnDropThunkDef)metaData.scriptClass->getMethod("InternalDoOnDrop")->getThunk();
	}

	void ScriptDropTarget::internal_CreateInstance(MonoObject* instance, ScriptEditorWindow* editorWindow)
	{
		ScriptDropTarget* nativeInstance = new (bs_alloc<ScriptDropTarget>()) ScriptDropTarget(instance, editorWindow->getEditorWidget());
	}

	void ScriptDropTarget::internal_Destroy(ScriptDropTarget* nativeInstance)
	{
		if (nativeInstance->mIsDestroyed)
			return;

		nativeInstance->destroy();
	}

	void ScriptDropTarget::internal_SetBounds(ScriptDropTarget* nativeInstance, Rect2I bounds)
	{
		if (nativeInstance->mIsDestroyed)
			return;

		nativeInstance->setBounds(bounds);
	}

	MonoArray* ScriptDropTarget::internal_GetFilePaths(ScriptDropTarget* nativeInstance)
	{
		OSDropTarget* dropTarget = nativeInstance->mDropTarget;

		if (nativeInstance->mIsDestroyed || dropTarget == nullptr || dropTarget->getDropType() != OSDropType::FileList)
			return ScriptArray::create<String>(0).getInternal();

		Vector<WString> fileList = dropTarget->getFileList();
		ScriptArray output = ScriptArray::create<WString>((UINT32)fileList.size());

		UINT32 idx = 0;
		for (auto& path : fileList)
		{
			output.set(idx, path);
			idx++;
		}

		return output.getInternal();
	}

	void ScriptDropTarget::destroy()
	{
		mIsDestroyed = true;

		mWidgetParentChangedConn.disconnect();
		mWidgetResizedConn.disconnect();

		setDropTarget(nullptr, 0, 0, 0, 0);
	}

	void ScriptDropTarget::setDropTarget(const RenderWindowPtr& parentWindow, INT32 x, INT32 y, UINT32 width, UINT32 height)
	{
		if (mDropTarget != nullptr)
		{
			Platform::destroyDropTarget(*mDropTarget);

			mDropTargetEnterConn.disconnect();
			mDropTargetLeaveConn.disconnect();
			mDropTargetMoveConn.disconnect();
			mDropTargetDroppedConn.disconnect();
		}

		if (parentWindow != nullptr)
		{
			mDropTarget = &Platform::createDropTarget(parentWindow.get(), x, y, width, height);

			mDropTargetEnterConn = mDropTarget->onEnter.connect(std::bind(&ScriptDropTarget::dropTargetDragMove, getManagedInstance(), _1, _2));
			mDropTargetMoveConn = mDropTarget->onDragOver.connect(std::bind(&ScriptDropTarget::dropTargetDragMove, getManagedInstance(), _1, _2));
			mDropTargetLeaveConn = mDropTarget->onLeave.connect(std::bind(&ScriptDropTarget::dropTargetDragLeave, getManagedInstance()));
			mDropTargetDroppedConn = mDropTarget->onDrop.connect(std::bind(&ScriptDropTarget::dropTargetDragDropped, getManagedInstance(), _1, _2));
		}
		else
			mDropTarget = nullptr;
	}

	void ScriptDropTarget::setBounds(const Rect2I& bounds)
	{
		mArea = bounds;

		Rect2I clippedArea = mArea;
		clippedArea.clip(mParentArea);

		if (mDropTarget != nullptr)
			mDropTarget->setArea(mParentArea.x + clippedArea.x, mParentArea.y + clippedArea.y, clippedArea.width, clippedArea.height);
	}

	void ScriptDropTarget::dropTargetDragEnter(MonoObject* instance, INT32 x, INT32 y)
	{
		MonoException* exception = nullptr;
		onEnterThunk(instance, x, y, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptDropTarget::dropTargetDragMove(MonoObject* instance, INT32 x, INT32 y)
	{
		MonoException* exception = nullptr;
		onMoveThunk(instance, x, y, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptDropTarget::dropTargetDragLeave(MonoObject* instance)
	{
		MonoException* exception = nullptr;
		onLeaveThunk(instance, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptDropTarget::dropTargetDragDropped(MonoObject* instance, INT32 x, INT32 y)
	{
		MonoException* exception = nullptr;
		onDropThunk(instance, x, y, &exception);

		MonoUtil::throwIfException(exception);
	}

	void ScriptDropTarget::widgetParentChanged(EditorWidgetContainer* parent)
	{
		RenderWindowPtr parentRenderWindow;
		if (parent != nullptr)
		{
			EditorWindowBase* parentWindow = parent->getParentWindow();

			if (parentWindow != nullptr)
				parentRenderWindow = parentWindow->getRenderWindow();
		}

		Rect2I clippedArea = mArea;
		if (parentRenderWindow == nullptr)
			mParentArea = Rect2I();
		
		clippedArea.clip(mParentArea);

		setDropTarget(parentRenderWindow, mParentArea.x + clippedArea.x, mParentArea.y + clippedArea.y, clippedArea.width, clippedArea.height);
	}

	void ScriptDropTarget::widgetResized(UINT32 width, UINT32 height)
	{
		mParentArea.width = width;
		mParentArea.height = height;

		setBounds(mArea);
	}
}