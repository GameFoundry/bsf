#include "Win32/BsVSCodeEditor.h"
#include <windows.h>
#include <atlbase.h>

// Import EnvDTE
#import "libid:80cc9f66-e7d8-4ddd-85b6-d9e6cd0e93e2" version("8.0") lcid("0") raw_interfaces_only named_guids

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

	LONG getRegistryStringValue(HKEY hKey, const WString& name, WString& value, const WString& defaultValue)
	{
		value = defaultValue;

		wchar_t strBuffer[512];
		DWORD strBufferSize = sizeof(strBuffer);
		ULONG result = RegQueryValueExW(hKey, name.c_str(), 0, nullptr, (LPBYTE)strBuffer, &strBufferSize);
		if (result == ERROR_SUCCESS)
			value = strBuffer;

		return result;
	}

	struct VSProjectInfo
	{
		WString GUID;
		WString name;
		Path path;
	}

	class VisualStudio
	{
	public:
		static CComPtr<EnvDTE::_DTE> findRunningInstance(const CLSID& clsID, const Path& solutionPath)
		{
			CComPtr<IRunningObjectTable> runningObjectTable = nullptr;
			if (FAILED(GetRunningObjectTable(0, &runningObjectTable)))
				return nullptr;

			CComPtr<IEnumMoniker> enumMoniker = nullptr;
			if (FAILED(runningObjectTable->EnumRunning(&enumMoniker)))
				return nullptr;

			CComPtr<IMoniker> dteMoniker = nullptr;
			if (FAILED(CreateClassMoniker(clsID, &dteMoniker)))
				return nullptr;

			CComBSTR bstrSolution(solutionPath.toWString(Path::PathType::Windows).c_str());
			CComPtr<IMoniker> moniker;
			ULONG count = 0;
			while (enumMoniker->Next(1, &moniker, &count) == S_OK)
			{
				if (moniker->IsEqual(dteMoniker))
				{
					CComPtr<IUnknown> curObject = nullptr;
					HRESULT result = runningObjectTable->GetObject(moniker, &curObject);
					moniker = nullptr;

					if (result != S_OK)
						continue;

					CComPtr<EnvDTE::_DTE> dte = curObject;
					if (dte == nullptr)
						return nullptr;

					CComPtr<EnvDTE::_Solution> solution;
					if (FAILED(dte->get_Solution(&solution)))
						continue;

					CComBSTR fullName;
					if (FAILED(solution->get_FullName(&fullName)))
						continue;

					if (fullName == bstrSolution)
						return dte;
				}
			}

			return nullptr;
		}

		static CComPtr<EnvDTE::_DTE> openInstance(const CLSID& clsid, const Path& solutionPath)
		{
			CComPtr<IUnknown> newInstance = nullptr;
			if (FAILED(::CoCreateInstance(clsid, nullptr, CLSCTX_LOCAL_SERVER, EnvDTE::IID__DTE, (LPVOID*)&newInstance)))
				return nullptr;

			CComPtr<EnvDTE::_DTE> dte = newInstance;
			if (dte == nullptr)
				return nullptr;

			dte->put_UserControl(TRUE);

			CComPtr<EnvDTE::_Solution> solution;
			if (FAILED(dte->get_Solution(&solution)))
				return nullptr;

			CComBSTR bstrSolution(solutionPath.toWString(Path::PathType::Windows).c_str());
			if (FAILED(solution->Open(bstrSolution)))
				return nullptr;

			// Wait until VS opens
			UINT32 elapsed = 0;
			while (elapsed < 10000)
			{
				EnvDTE::Window* window = nullptr;
				if (SUCCEEDED(dte->get_MainWindow(&window)))
					return dte;

				Sleep(100);
				elapsed += 100;
			}

			return nullptr;
		}

		static bool openFile(CComPtr<EnvDTE::_DTE> dte, const Path& filePath, UINT32 line)
		{
			// Open file
			CComPtr<EnvDTE::ItemOperations> itemOperations;
			if (FAILED(dte->get_ItemOperations(&itemOperations)))
				return false;

			CComBSTR bstrFilePath(filePath.toWString(Path::PathType::Windows).c_str());
			CComBSTR bstrKind(EnvDTE::vsViewKindPrimary);
			CComPtr<EnvDTE::Window> window = nullptr;
			if (FAILED(itemOperations->OpenFile(bstrFilePath, bstrKind, &window)))
				return false;

			// Scroll to line
			CComPtr<EnvDTE::Document> activeDocument;
			if (SUCCEEDED(dte->get_ActiveDocument(&activeDocument)))
			{
				CComPtr<IDispatch> selection;
				if (SUCCEEDED(activeDocument->get_Selection(&selection)))
				{
					CComPtr<EnvDTE::TextSelection> textSelection;
					if (SUCCEEDED(selection->QueryInterface(&textSelection)))
					{
						textSelection->GotoLine(line, TRUE);
					}
				}
			}

			// Bring the window in focus
			window = nullptr;
			if (SUCCEEDED(dte->get_MainWindow(&window)))
			{
				window->Activate();

				HWND hWnd;
				window->get_HWnd((LONG*)&hWnd);
				SetForegroundWindow(hWnd);
			}

			return true;
		}

		static WString writeSolution(VisualStudioVersion version, const WString& solutionGUID, const Vector<VSProjectInfo>& projects)
		{
			WStringStream stream;


			const WString header;
			
		}
	};

	VSCodeEditor::VSCodeEditor(const Path& execPath, const WString& CLSID)
		:mCLSID(CLSID), mExecPath(execPath)
	{
		
	}

	void VSCodeEditor::openFile(const Path& solutionPath, const Path& filePath, UINT32 lineNumber) const
	{
		CLSID clsID;
		if (FAILED(CLSIDFromString(mCLSID.toWString().c_str(), &clsID)))
			return;

		CComPtr<EnvDTE::_DTE> dte = VisualStudio::findRunningInstance(clsID, solutionPath);
		if (dte == nullptr)
			dte = VisualStudio::openInstance(clsID, solutionPath);

		if (dte == nullptr)
			return;

		VisualStudio::openFile(dte, filePath, lineNumber);
	}

	void VSCodeEditor::syncSolution(const CodeSolutionData& data, const Path& outputPath) const
	{
		// TODO
	}

	VSCodeEditorFactory::VSCodeEditorFactory()
		:mAvailableVersions(getAvailableVersions())
	{ 
		for (auto& version : mAvailableVersions)
			mAvailableEditors.push_back(version.first);
	}

	Map<WString, VSCodeEditorFactory::VSVersionInfo> VSCodeEditorFactory::getAvailableVersions() const
	{
#if BS_ARCH_TYPE == BS_ARCHITECTURE_x86_64
		bool is64bit = true;
#else
		bool is64bit = false;
		IsWow64Process(GetCurrentProcess(), &is64bit);
#endif

		WString registryKeyRoot;
		if (is64bit)
			registryKeyRoot = L"SOFTWARE\\Microsoft";
		else
			registryKeyRoot = L"SOFTWARE\\Wow6432Node\\Microsoft";

		struct VersionData
		{
			WString registryKey;
			WString name;
			WString executable;
		};

		Map<VisualStudioVersion, VersionData> versionToVersionNumber =
		{ 
			{ VisualStudioVersion::VS2008, { L"VisualStudio\\9.0", L"Visual Studio 2008", L"devenv.exe" } },
			{ VisualStudioVersion::VS2010, { L"VisualStudio\\10.0", L"Visual Studio 2010", L"devenv.exe" } },
			{ VisualStudioVersion::VS2012, { L"VisualStudio\\11.0", L"Visual Studio 2012", L"devenv.exe" } },
			{ VisualStudioVersion::VS2013, { L"VisualStudio\\12.0", L"Visual Studio 2013", L"devenv.exe" } },
			{ VisualStudioVersion::VS2015, { L"VisualStudio\\13.0", L"Visual Studio 2015", L"devenv.exe" } }
		};

		Map<WString, VSVersionInfo> versionInfo;
		for(auto version : versionToVersionNumber)
		{
			WString registryKey = registryKeyRoot + L"\\" + version.second.registryKey;

			HKEY regKey;
			LONG result = RegOpenKeyExW(HKEY_LOCAL_MACHINE, registryKeyRoot.c_str(), 0, KEY_READ, &regKey);
			if (result != ERROR_SUCCESS)
				continue;

			WString installPath;
			getRegistryStringValue(regKey, L"InstallDir", installPath, StringUtil::WBLANK);
			if (installPath.empty())
				continue;

			WString clsID;
			getRegistryStringValue(regKey, L"ThisVersionDTECLSID", clsID, StringUtil::WBLANK);

			VSVersionInfo info;
			info.name = version.second.name;
			info.execPath = installPath.append(version.second.executable);
			info.CLSID = clsID;

			versionInfo[info.name] = info;
		}

		// TODO - Also query for VSExpress and VSCommunity (their registry keys are different)

		return versionInfo;
	}

	CodeEditor* VSCodeEditorFactory::create(const WString& editor) const
	{
		auto findIter = mAvailableVersions.find(editor);
		if (findIter == mAvailableVersions.end())
			return nullptr;

		// TODO - Also create VSExpress and VSCommunity editors

		return bs_new<VSCodeEditor>(findIter->second.execPath, findIter->second.CLSID);
	}
}