#include "BsCodeEditor.h"

#if BS_PLATFORM == BS_PLATFORM_WIN32
#include "Win32/BsVSCodeEditor.h"
#else
static_assert("Make sure to add implementations for other platforms.");
#endif

namespace BansheeEngine
{
	CodeEditorManager::CodeEditorManager()
		:mActiveEditor(nullptr)
	{
#if BS_PLATFORM == BS_PLATFORM_WIN32
		VSCodeEditorFactory* vsCodeEditorFactory = bs_new<VSCodeEditorFactory>();
		Vector<WString> vsEditors = vsCodeEditorFactory->getAvailableEditors();
		for(auto& editor : vsEditors)
		{
			mFactoryPerEditor[editor] = vsCodeEditorFactory;
			mEditors.push_back(editor);
		}

		mFactories.push_back(vsCodeEditorFactory);
#else
		static_assert("Make sure to add implementations for other platforms.");
#endif
	}

	CodeEditorManager::~CodeEditorManager()
	{
		for (auto& factory : mFactories)
			bs_delete(factory);

		if (mActiveEditor != nullptr)
			bs_delete(mActiveEditor);
	}

	void CodeEditorManager::setActive(const WString& editor)
	{
		if (mActiveEditor != nullptr)
		{
			bs_delete(mActiveEditor);
			mActiveEditor = nullptr;
		}

		auto findIter = mFactoryPerEditor.find(editor);
		if (findIter == mFactoryPerEditor.end())
			return;

		mActiveEditor = findIter->second->create(editor);
	}

	void CodeEditorManager::openFile(const Path& path, UINT32 lineNumber) const
	{
		if (mActiveEditor != nullptr)
			mActiveEditor->openFile(path, lineNumber);
	}

	void CodeEditorManager::syncSolution(const CodeSolutionData& data, const Path& outputPath) const
	{
		if (mActiveEditor != nullptr)
			mActiveEditor->syncSolution(data, outputPath);
	}
}