//__________________________ Banshee Project - A modern game development toolkit _________________________________//
//_____________________________________ www.banshee-project.com __________________________________________________//
//________________________ Copyright (c) 2014 Marko Pintera. All rights reserved. ________________________________//
#pragma once

#include "Shellapi.h"

// This is just a helper include for BsPlatformImpl.cpp, it's not meant to be used on its own

namespace BansheeEngine
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
		/**
		 * @brief	Type of drag and drop event.
		 */
		enum class DropOpType
		{
			DragOver,
			Drop,
			Leave
		};

		/**
		 * @brief	Type of data that a drag and drop operation contains.
		 */
		enum class DropOpDataType
		{
			FileList,
			None
		};

		/**
		 * @brief	Structure describing a drag and drop operation.
		 */
		struct DropTargetOp
		{
			DropTargetOp(DropOpType _type, const Vector2I& _pos)
				:type(_type), position(_pos), dataType(DropOpDataType::None)
			{ }

			DropOpType type;
			Vector2I position;

			DropOpDataType dataType;
			union 
			{
				Vector<WString>* mFileList;
			};
		};

	public:
		Win32DropTarget(HWND hWnd)
			:mHWnd(hWnd), mRefCount(1), mAcceptDrag(false)
		{ }

		~Win32DropTarget()
		{
			BS_LOCK_MUTEX(mSync);

			for(auto& fileList : mFileLists)
			{
				bs_delete(fileList);
			}

			mFileLists.clear();
			mQueuedDropOps.clear();
		}

		/**
		 * @brief	Registers the drop target with the operating system. Monitoring
		 * 			for drag and drop operations starts.
		 */
		void registerWithOS()
		{
			CoLockObjectExternal(this, TRUE, FALSE);
			HRESULT hr = RegisterDragDrop(mHWnd, this);
		}

		/**
		 * @brief	Unregisters the drop target with the operating system. Monitoring
		 * 			for drag and drop operations stops.
		 */
		void unregisterWithOS()
		{
			RevokeDragDrop(mHWnd);
			CoLockObjectExternal(this, FALSE, FALSE);
		}

		/**
		 * @brief	COM requirement. Returns instance of an interface of
		 * 			provided type.
		 */
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

		/**
		 * @brief	COM requirement. Increments objects
		 * 			reference count.
		 */
		ULONG __stdcall AddRef()
		{
			return InterlockedIncrement(&mRefCount);
		} 

		/**
		 * @brief	COM requirement. Decreases the objects 
		 * 			reference count and deletes the object
		 * 			if its zero.
		 */
		ULONG __stdcall Release()
		{
			LONG count = InterlockedDecrement(&mRefCount);

			if(count == 0)
			{
				bs_delete(this);
				return 0;
			}
			else
			{
				return count;
			}
		} 

		/**
		 * @brief	Called by the OS when user enters the drop target area while dragging an object.
		 * 			
		 * @note	Called on core thread.
		 */
		HRESULT __stdcall DragEnter(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_LINK;

			mAcceptDrag = isDataValid(pDataObj);
			if(!mAcceptDrag)
				return S_OK;

			{
				BS_LOCK_MUTEX(mSync);

				mFileLists.push_back(getFileListFromData(pDataObj));

				ScreenToClient(mHWnd, (POINT *)&pt);
				mQueuedDropOps.push_back(DropTargetOp(DropOpType::DragOver, Vector2I((int)pt.x, (int)pt.y)));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}
			
			return S_OK;
		}

		/**
		 * @brief	Called by the OS while user continues to drag an object over the drop target.
		 * 			
		 * @note	Called on core thread.
		 */
		HRESULT __stdcall DragOver(DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_LINK;

			if(!mAcceptDrag)
				return S_OK;

			{
				BS_LOCK_MUTEX(mSync);

				ScreenToClient(mHWnd, (POINT *)&pt);
				mQueuedDropOps.push_back(DropTargetOp(DropOpType::DragOver, Vector2I((int)pt.x, (int)pt.y)));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}
			
			return S_OK;
		} 

		/**
		 * @brief	Called by the OS when user leaves the drop target.
		 * 			
		 * @note	Called on core thread.
		 */
		HRESULT __stdcall DragLeave()
		{
			{
				BS_LOCK_MUTEX(mSync);

				mQueuedDropOps.push_back(DropTargetOp(DropOpType::Leave, Vector2I()));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}

			return S_OK;
		}

		/**
		 * @brief	Called by the OS when the user ends the drag operation while
		 * 			over the drop target.
		 * 			
		 * @note	Called on core thread.
		 */
		HRESULT __stdcall Drop(IDataObject* pDataObj, DWORD grfKeyState, POINTL pt, DWORD* pdwEffect)
		{
			*pdwEffect = DROPEFFECT_LINK;
			mAcceptDrag = false;

			if(!isDataValid(pDataObj))
				return S_OK;

			{
				BS_LOCK_MUTEX(mSync);

				mFileLists.push_back(getFileListFromData(pDataObj));

				ScreenToClient(mHWnd, (POINT *)&pt);
				mQueuedDropOps.push_back(DropTargetOp(DropOpType::Drop, Vector2I((int)pt.x, (int)pt.y)));

				DropTargetOp& op = mQueuedDropOps.back();
				op.dataType = DropOpDataType::FileList;
				op.mFileList = mFileLists.back();
			}

			return S_OK;
		}

		/**
		 * @brief	Registers a new drop target to monitor.
		 *
		 * @note	Sim thread only.
		 */
		void registerDropTarget(OSDropTarget* dropTarget)
		{
			mDropTargets.push_back(dropTarget);
		}

		/**
		 * @brief	Unregisters an existing drop target and stops monitoring it.
		 *
		 * @note	Sim thread only.
		 */
		void unregisterDropTarget(OSDropTarget* dropTarget)
		{
			auto findIter = std::find(begin(mDropTargets), end(mDropTargets), dropTarget);
			if(findIter != mDropTargets.end())
				mDropTargets.erase(findIter);
		}

		/**
		 * @brief	Gets the total number of monitored drop targets.
		 * 			
		 * @note	Sim thread only.
		 */
		unsigned int getNumDropTargets() const 
		{ 
			return (unsigned int)mDropTargets.size(); 
		}

		/**
		 * @brief	Called every frame by the sim thread. Internal method.
		 */
		void update()
		{
			BS_LOCK_MUTEX(mSync);

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
					bs_delete(op.mFileList);
					mFileLists.erase(mFileLists.begin());
				}
			}

			mQueuedDropOps.clear();
		}
	private:
		/**
		 * @brief	Check if we support the data in the provided drag and drop data object.
		 */
		bool isDataValid(IDataObject* data)
		{
			// TODO - Currently only supports file drag and drop, so only CF_HDROP is used
			FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };

			return data->QueryGetData(&fmtetc) == S_OK ? true : false;
		}

		/**
		 * @brief	Gets a file list from data. Caller must ensure that the data actually
		 * 			contains a file list.
		 */
		Vector<WString>* getFileListFromData(IDataObject* data)
		{
			FORMATETC fmtetc = { CF_HDROP, 0, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
			STGMEDIUM stgmed;

			Vector<WString>* files = bs_new<Vector<WString>>();
			if(data->GetData(&fmtetc, &stgmed) == S_OK)
			{
				PVOID data = GlobalLock(stgmed.hGlobal);

				HDROP hDrop = (HDROP)data;
				UINT numFiles = DragQueryFileW(hDrop, 0xFFFFFFFF, nullptr, 0);

				files->resize(numFiles);
				for(UINT i = 0; i < numFiles; i++)
				{
					UINT numChars = DragQueryFileW(hDrop, i, nullptr, 0) + 1;
					wchar_t* buffer = (wchar_t*)bs_alloc((UINT32)numChars * sizeof(wchar_t));

					DragQueryFileW(hDrop, i, buffer, numChars);

					(*files)[i] = WString(buffer);

					bs_free(buffer);
				}

				GlobalUnlock(stgmed.hGlobal);
				ReleaseStgMedium(&stgmed);
			}

			return files;
		}

	private:
		Vector<OSDropTarget*> mDropTargets;

		LONG mRefCount;
		HWND mHWnd;
		bool mAcceptDrag;

		Vector<DropTargetOp> mQueuedDropOps;
		Vector<Vector<WString>*> mFileLists; 

		BS_MUTEX(mSync);
	};
}