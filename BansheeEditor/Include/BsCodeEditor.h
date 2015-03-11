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
		Vector<CodeProjectReference> assemblyReferences;
		Vector<CodeProjectReference> projectReferences;
	};

	struct BS_ED_EXPORT CodeSolutionData
	{
		WString name;
		Vector<CodeProjectData> projects;
	};

	class BS_ED_EXPORT CodeEditorManager : public Module<CodeEditorManager>
	{
	public:
		CodeEditorManager();
		~CodeEditorManager();

		const Vector<CodeEditorType>& getAvailableEditors() const { return mEditors; }
		void setActive(CodeEditorType editor);

		void openFile(const Path& path, UINT32 lineNumber) const;
		void syncSolution() const;

	private:
		Path getSolutionPath() const;

		CodeEditor* mActiveEditor;
		Map<CodeEditorType, CodeEditorFactory*> mFactoryPerEditor;
		Vector<CodeEditorType> mEditors;
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
		virtual const Vector<CodeEditorType>& getAvailableEditors() const = 0;
		virtual CodeEditor* create(CodeEditorType editor) const = 0;
	};
}