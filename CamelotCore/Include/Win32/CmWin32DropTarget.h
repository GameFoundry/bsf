#pragma once

#include "Shellapi.h"

// This is just a helper include for CmPlatformImpl.cpp, it's not meant to be used on its own

namespace CamelotFramework
{
	/**
	* @brief	Called by the OS when various drag and drop actions are performed over a
	* 			window this control is registered for.
	* 			
	* @note		This class queues all messages receives by the OS (from the core thread), and then executes
	* 			the queue on sim thread. You should be wary of which methods are allowed to be called from which
	* 			thread.
	*/
	class Win32DropTarget : public IDropTarget
	{
	public:
		Win32DropTarget(HWND hWnd)
			:mHWnd(hWnd), mRefCount(1), mAcceptDrag(false)
		{ }

		~Win32DropTarget()
		{ }

		void registerWithOS()
		{
			CoLockObjectExternal(this, TRUE, FALSE);
			RegisterDragDrop(mHWnd, this);
		}

		void unregisterWithOS()
		{
			RevokeDragDrop(mHWnd);
			CoLockObjectExternal(this, FALSE, FALSE);
		}

		HRESULT __stdcall QueryInterface(REFIID iid, void** ppvObject)
		{
			if(iid == IID_IDropTarget || iid == IID_IUnknown)
			{
				AddRef();
				*ppvObject = this;
				return S_OK;
			}
			else
			{
				*ppvObject = nullptr;
				return E_NOINTERFACE;
			}
		}

		ULONG __stdcall AddRef()
		{
			return InterlockedIncrement(&mRefCount);
		} 

		ULONG __stdcall Release()
		{
			LONG count = InterlockedDecrement(&mRefCount);

			if(count == 0)
			{
				cm_delete(this);
				return 0;
			}
			else
			{
				return count;
			}
		} 

		HRESULT __stdcall DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_NONE;

			mAcceptDrag = isDataValid(pDataObj);
			if(!mAcceptDrag)
				return S_OK;
			
			// TODO - Queue (or set) move command to use later in update()

			return S_OK;
		}

		HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_NONE;

			if(!mAcceptDrag)
				return S_OK;

			// TODO - Queue (or set) move command to use later in update()
			
			return S_OK;
		} 

		HRESULT __stdcall DragLeave()
		{
			// Do nothing

			return S_OK;
		}

		HRESULT __stdcall Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_NONE;

			if(!isDataValid(pDataObj))
				return S_OK;

			Vector<WString>::type fileList = getFileListFromData(pDataObj);

			// TODO - Add drop operation to a queue, and process it in update()

			return S_OK;
		}

		// Sim thread only
		void registerDropTarget(OSDropTarget* dropTarget)
		{
			mDropTargets.push_back(dropTarget);
		}

		void unregisterDropTarget(OSDropTarget* dropTarget)
		{
			auto findIter = std::find(begin(mDropTargets), end(mDropTargets), dropTarget);
			if(findIter != mDropTargets.end())
				mDropTargets.erase(findIter);
		}

		unsigned int getNumDropTargets() const 
		{ 
			return (unsigned int)mDropTargets.size(); 
		}

		/**
		 * @brief	Called every frame by the sim thread. Internal method.
		 */
		void update()
		{
			// TODO - Process received data and notify child OSDropTargets
		}
	private:
		bool isDataValid(IDataObject* data)
		{
			// TODO - Currently only supports file drag and drop, so only CF_HDROP is used
			FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

			return data->QueryGetData(&fmtetc) == S_OK ? true : false;
		}

		Vector<WString>::type getFileListFromData(IDataObject* data)
		{
			FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM stgmed;

			Vector<WString>::type files;
			if(data->GetData(&fmtetc, &stgmed) == S_OK)
			{
				PVOID data = GlobalLock(stgmed.hGlobal);

				HDROP hDrop = (HDROP)data;
				UINT numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

				files.resize(numFiles);
				for(UINT i = 0; i < numFiles; i++)
				{
					UINT numChars = DragQueryFileW(hDrop, i, nullptr, 0) + 1;
					wchar_t* buffer = (wchar_t*)cm_alloc((UINT32)numChars * sizeof(wchar_t));

					DragQueryFileW(hDrop, i, buffer, numChars);

					files[i] = WString(buffer);
				}

				GlobalUnlock(stgmed.hGlobal);
				ReleaseStgMedium(&stgmed);
			}

			return files;
		}

	private:
		Vector<OSDropTarget*>::type mDropTargets;

		LONG mRefCount;
		HWND mHWnd;
		bool mAcceptDrag;
	};
}