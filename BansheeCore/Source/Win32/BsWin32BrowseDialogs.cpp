#include "BsPlatform.h"
#include "BsPath.h"
#include <atlbase.h>
#include <ShObjIdl.h>

namespace BansheeEngine
{
	void addFiltersToDialog(IFileDialog* fileDialog, const WString& filterList)
	{
		const wchar_t EMPTY_WSTR[] = L"";
		const wchar_t WILDCARD[] = L"*.*";

		if (filterList.empty())
			return;

		Vector<WString> filters = StringUtil::split(filterList, L";");
		UINT32 numFilters = (UINT32)filters.size();
		if (numFilters == 0)
			return;

		COMDLG_FILTERSPEC* specList = bs_newN<COMDLG_FILTERSPEC>(numFilters);
		for (size_t i = 0; i < numFilters; ++i)
		{
			specList[i].pszName = EMPTY_WSTR;
			specList[i].pszSpec = filters[i].c_str();
		}

		fileDialog->SetFileTypes(numFilters, specList);

		bs_deleteN(specList, numFilters);
	}

	void setDefaultPath(IFileDialog* dialog, const Path& defaultPath)
	{
		const wchar_t* defaultPathW = defaultPath.toWString().c_str();

		IShellItem* folder;
		HRESULT result = SHCreateItemFromParsingName(defaultPathW, NULL, IID_PPV_ARGS(&folder));

		// Valid non results.
		if (result == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) || result == HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE))
			return;

		if (!SUCCEEDED(result))
			return;

		dialog->SetFolder(folder);
		folder->Release();
	}

	void getPaths(IShellItemArray* shellItems, Vector<Path>& paths)
	{
		DWORD numShellItems;
		shellItems->GetCount(&numShellItems);

		for (DWORD i = 0; i < numShellItems; ++i)
		{
			IShellItem* shellItem = nullptr;
			shellItems->GetItemAt(i, &shellItem);

			SFGAOF attribs;
			shellItem->GetAttributes(SFGAO_FILESYSTEM, &attribs);

			if (!(attribs & SFGAO_FILESYSTEM))
				continue;

			LPWSTR name;
			shellItem->GetDisplayName(SIGDN_FILESYSPATH, &name);
			paths.push_back((Path)name);
			CoTaskMemFree(name);
		}
	}

	bool Platform::openBrowseDialog(FileDialogType type, const Path& defaultPath, const WString& filterList,
		Vector<Path>& paths)
	{
		// Init COM library.
		CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

		IFileDialog* fileDialog = nullptr;

		UINT32 dialogType = ((UINT32)type & (UINT32)FileDialogType::TypeMask);
		bool isOpenDialog = dialogType == (UINT32)FileDialogType::OpenFile || dialogType == (UINT32)FileDialogType::OpenFolder;

		// Create dialog
		IID classId = isOpenDialog ? CLSID_FileOpenDialog : CLSID_FileSaveDialog;
		CoCreateInstance(classId, nullptr, CLSCTX_ALL, IID_PPV_ARGS(&fileDialog));

		addFiltersToDialog(fileDialog, filterList);
		setDefaultPath(fileDialog, defaultPath);

		// Apply multiselect flags
		bool isMultiselect = false;
		if (isOpenDialog)
		{
			if (dialogType == (UINT32)FileDialogType::OpenFile)
			{
				if (((UINT32)type & (UINT32)FileDialogType::Multiselect) != 0)
				{
					DWORD dwFlags;
					fileDialog->GetOptions(&dwFlags);
					fileDialog->SetOptions(dwFlags | FOS_ALLOWMULTISELECT);

					isMultiselect = true;
				}
			}
			else
			{
				DWORD dwFlags;
				fileDialog->GetOptions(&dwFlags);
				fileDialog->SetOptions(dwFlags | FOS_PICKFOLDERS);
			}
		}

		// Show the dialog
		bool finalResult = false;
		if (SUCCEEDED(fileDialog->Show(nullptr)))
		{
			if (isMultiselect)
			{
				// Get file names
				IFileOpenDialog* fileOpenDialog = static_cast<IFileOpenDialog*>(fileDialog);

				IShellItemArray* shellItems = nullptr;
				fileOpenDialog->GetResults(&shellItems);

				getPaths(shellItems, paths);
				shellItems->Release();
			}
			else
			{
				// Get the file name
				IShellItem* shellItem = nullptr;
				fileDialog->GetResult(&shellItem);

				LPWSTR filePath = nullptr;
				shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);

				paths.push_back((Path)filePath);
				CoTaskMemFree(filePath);

				shellItem->Release();
			}

			finalResult = true;
		}

		CoUninitialize();
		return finalResult;
	}
}