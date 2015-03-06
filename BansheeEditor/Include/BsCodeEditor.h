#pragma once

#include "BsEditorPrerequisites.h"
#include "BsModule.h"

namespace BansheeEngine
{
	class CodeEditor;
	class CodeEditorFactory;

	struct BS_ED_EXPORT CodeProjectReference
	{
		WString name;
		Path path;
	};

	struct BS_ED_EXPORT CodeProjectData
	{
		WString name;
		Vector<Path> codeFiles;
		Vector<Path> nonCodeFiles;
		Vector<WString> defines;
		Vector<CodeProjectReference> references;
	};

	struct BS_ED_EXPORT CodeSolutionData
	{
		Vector<CodeProjectData> projects;
	};

	class BS_ED_EXPORT CodeEditorManager : public Module<CodeEditorManager>
	{
	public:
		CodeEditorManager();
		~CodeEditorManager();

		const Vector<WString>& getAvailableEditors() const { return mEditors; }
		void setActive(const WString& editor);

		void openFile(const Path& path, UINT32 lineNumber) const;
		void syncSolution(const CodeSolutionData& data, const Path& outputPath) const; // TODO - Need some kind of project information like: name, list of files, list of references, possibly defines and other?

	private:
		CodeEditor* mActiveEditor;
		Map<WString, CodeEditorFactory*> mFactoryPerEditor;
		Vector<WString> mEditors;
		Vector<CodeEditorFactory*> mFactories;
	};

	class BS_ED_EXPORT CodeEditor
	{
	public:
		virtual void openFile(const Path& solutionPath, const Path& path, UINT32 lineNumber) const = 0;
		virtual void syncSolution(const CodeSolutionData& data, const Path& outputPath) const = 0;
	};

	class BS_ED_EXPORT CodeEditorFactory
	{
	public:
		virtual const Vector<WString>& getAvailableEditors() const = 0;
		virtual CodeEditor* create(const WString& editor) const = 0;
	};
}