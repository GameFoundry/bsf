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
		enum class DropOpType
		{
			DragOver,
			Drop,
			Leave
		};

		enum class DropOpDataType
		{
			FileList,
			None
		};

		struct DropTargetOp
		{
			DropTargetOp(DropOpType _type, const Int2& _pos)
				:type(_type), position(_pos), dataType(DropOpDataType::None)
			{ }

			DropOpType type;
			Int2 position;

			DropOpDataType dataType;
			union 
			{
				Vector<WString>::type* mFileList;
			};
		};

	public:
		Win32DropTarget(HWND hWnd)
			:mHWnd(hWnd), mRefCount(1), mAcceptDrag(false)
		{ }

		~Win32DropTarget()
		{
			CM_LOCK_MUTEX(mSync);

			for(auto& fileList : mFileLists)
			{
				cm_delete(fileList);
			}

			mFileLists.clear();
			mQueuedDropOps.clear();
		}

		void registerWithOS()
		{
			CoLockObjectExternal(this, TRUE, FALSE);
			HRESULT hr = RegisterDragDrop(mHWnd, this);
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
			*pdwEffect = DROPEFFECT_LINK;

			mAcceptDrag = isDataValid(pDataObj);
			if(!mAcceptDrag)
				return S_OK;

			{
				CM_LOCK_MUTEX(mSync);

				mFileLists.push_back(getFileListFromData(pDataObj));

				ScreenToClient(mHWnd, (POINT *)&pt);
				mQueuedDropOps.push_back(DropTargetOp(DropOpType::DragOver, Int2((int)pt.x, (int)pt.y)));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}
			
			return S_OK;
		}

		HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_LINK;

			if(!mAcceptDrag)
				return S_OK;

			{
				CM_LOCK_MUTEX(mSync);

				ScreenToClient(mHWnd, (POINT *)&pt);
				mQueuedDropOps.push_back(DropTargetOp(DropOpType::DragOver, Int2((int)pt.x, (int)pt.y)));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}
			
			return S_OK;
		} 

		HRESULT __stdcall DragLeave()
		{
			{
				CM_LOCK_MUTEX(mSync);

				mQueuedDropOps.push_back(DropTargetOp(DropOpType::Leave, Int2()));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}

			return S_OK;
		}

		HRESULT __stdcall Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_LINK;
			mAcceptDrag = false;

			if(!isDataValid(pDataObj))
				return S_OK;

			{
				CM_LOCK_MUTEX(mSync);

				mFileLists.push_back(getFileListFromData(pDataObj));

				ScreenToClient(mHWnd, (POINT *)&pt);
				mQueuedDropOps.push_back(DropTargetOp(DropOpType::Drop, Int2((int)pt.x, (int)pt.y)));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}

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
			CM_LOCK_MUTEX(mSync);

			for(auto& op: mQueuedDropOps)
			{
				for(auto& target : mDropTargets)
				{
					if(op.type != DropOpType::Leave)
					{
						if(target->_isInside(op.position))
						{
							if(!target->_isActive())
							{
								target->_setFileList(*op.mFileList);
								target->onEnter(op.position.x, op.position.y);
							}

							if(op.type == DropOpType::DragOver)
								target->onDragOver(op.position.x, op.position.y);
							else if(op.type == DropOpType::Drop)
							{
								target->_setFileList(*op.mFileList);
								target->onDrop(op.position.x, op.position.y);
							}
						}
						else
						{
							if(target->_isActive())
							{
								target->onLeave();
								target->_clear();
								target->_setActive(false);
							}
						}
					}
					else
					{
						if(target->_isActive())
						{
							target->onLeave();
							target->_clear();
							target->_setActive(false);
						}
					}
				}

				if(op.type == DropOpType::Leave || op.type == DropOpType::Drop)
				{
					cm_delete(op.mFileList);
					mFileLists.erase(mFileLists.begin());
				}
			}

			mQueuedDropOps.clear();
		}
	private:
		bool isDataValid(IDataObject* data)
		{
			// TODO - Currently only supports file drag and drop, so only CF_HDROP is used
			FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

			return data->QueryGetData(&fmtetc) == S_OK ? true : false;
		}

		Vector<WString>::type* getFileListFromData(IDataObject* data)
		{
			FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM stgmed;

			Vector<WString>::type* files = cm_new<Vector<WString>::type>();
			if(data->GetData(&fmtetc, &stgmed) == S_OK)
			{
				PVOID data = GlobalLock(stgmed.hGlobal);

				HDROP hDrop = (HDROP)data;
				UINT numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

				files->resize(numFiles);
				for(UINT i = 0; i < numFiles; i++)
				{
					UINT numChars = DragQueryFileW(hDrop, i, nullptr, 0) + 1;
					wchar_t* buffer = (wchar_t*)cm_alloc((UINT32)numChars * sizeof(wchar_t));

					DragQueryFileW(hDrop, i, buffer, numChars);

					(*files)[i] = WString(buffer);

					cm_free(buffer);
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

		Vector<DropTargetOp>::type mQueuedDropOps;
		Vector<Vector<WString>::type*>::type mFileLists; 

		CM_MUTEX(mSync);
	};
}