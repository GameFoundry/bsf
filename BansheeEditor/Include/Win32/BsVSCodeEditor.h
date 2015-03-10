#pragma once

#include "BsEditorPrerequisites.h"
#include "BsCodeEditor.h"

namespace BansheeEngine
{
	enum class VisualStudioVersion
	{
		VS2008,
		VS2010,
		VS2012,
		VS2013,
		VS2015
	};

	class BS_ED_EXPORT VSCodeEditor : public CodeEditor
	{
	public:
		VSCodeEditor(VisualStudioVersion version, const Path& execPath, const WString& CLSID);

		void openFile(const Path& solutionPath, const Path& filePath, UINT32 lineNumber) const override;
		void syncSolution(const CodeSolutionData& data, const Path& outputPath) const override;

	private:
		VisualStudioVersion mVersion;
		Path mExecPath;
		WString mCLSID;
	};

	class BS_ED_EXPORT VSCodeEditorFactory : public CodeEditorFactory
	{
	public:
		VSCodeEditorFactory();

		const Vector<WString>& getAvailableEditors() const override { return mAvailableEditors; }
		CodeEditor* create(const WString& editor) const override;

	private:
		struct VSVersionInfo
		{
			WString name;
			Path execPath;
			WString CLSID;
			VisualStudioVersion version;
		};

		Map<WString, VSVersionInfo> getAvailableVersions() const;

		Map<WString, VSVersionInfo> mAvailableVersions;
		Vector<WString> mAvailableEditors;
	};
}