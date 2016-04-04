//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "BsEditorPrerequisites.h"
#include "BsEditorSettings.h"
#include "BsRTTIType.h"

namespace BansheeEngine
{
	/** @cond RTTI */
	/** @addtogroup RTTI-Impl-Editor
	 *  @{
	 */

	class EditorSettingsRTTI : public RTTIType <EditorSettings, Settings, EditorSettingsRTTI>
	{
	private:
		BS_PLAIN_MEMBER(mMoveSnapActive);
		BS_PLAIN_MEMBER(mRotateSnapActive);

		BS_PLAIN_MEMBER(mMoveSnap);
		BS_PLAIN_MEMBER(mRotationSnap);

		BS_PLAIN_MEMBER(mGridSize);
		BS_PLAIN_MEMBER(mGridAxisSpacing);

		BS_PLAIN_MEMBER(mActiveSceneTool);
		BS_PLAIN_MEMBER(mActiveCoordinateMode);
		BS_PLAIN_MEMBER(mActivePivotMode);

		BS_PLAIN_MEMBER(mHandleSize);

		BS_PLAIN_MEMBER(mLastOpenProject);
		BS_PLAIN_MEMBER(mAutoLoadLastProject);
		BS_PLAIN_MEMBER(mRecentProjects);

		BS_PLAIN_MEMBER(mFPSLimit);
		BS_PLAIN_MEMBER(mMouseSensitivity);

	public:
		EditorSettingsRTTI()
		{
			BS_ADD_PLAIN_FIELD(mMoveSnapActive, 0);
			BS_ADD_PLAIN_FIELD(mRotateSnapActive, 1);

			BS_ADD_PLAIN_FIELD(mMoveSnap, 2);
			BS_ADD_PLAIN_FIELD(mRotationSnap, 3);

			BS_ADD_PLAIN_FIELD(mGridSize, 4);
			BS_ADD_PLAIN_FIELD(mGridAxisSpacing, 5);

			BS_ADD_PLAIN_FIELD(mActiveSceneTool, 6);
			BS_ADD_PLAIN_FIELD(mActiveCoordinateMode, 7);
			BS_ADD_PLAIN_FIELD(mActivePivotMode, 8);

			BS_ADD_PLAIN_FIELD(mHandleSize, 9);

			BS_ADD_PLAIN_FIELD(mLastOpenProject, 10);
			BS_ADD_PLAIN_FIELD(mAutoLoadLastProject, 11);
			BS_ADD_PLAIN_FIELD(mRecentProjects, 12);

			BS_ADD_PLAIN_FIELD(mFPSLimit, 13);
			BS_ADD_PLAIN_FIELD(mMouseSensitivity, 14);
		}

		const String& getRTTIName() override
		{
			static String name = "EditorSettings";
			return name;
		}

		UINT32 getRTTIId() override
		{
			return TID_EditorSettings;
		}

		SPtr<IReflectable> newRTTIObject() override
		{
			return bs_shared_ptr_new<EditorSettings>();
		}
	};

	template<> struct RTTIPlainType <RecentProject>
	{
		enum { id = TID_RecentProject }; enum { hasDynamicSize = 1 };

		static void toMemory(const RecentProject& data, char* memory)
		{
			UINT32 size = 0;
			char* memoryStart = memory;
			memory += sizeof(UINT32);
			size += sizeof(UINT32);

			memory = rttiWriteElem(data.path, memory, size);
			memory = rttiWriteElem(data.accessTimestamp, memory, size);

			memcpy(memoryStart, &size, sizeof(UINT32));
		}

		static UINT32 fromMemory(RecentProject& data, char* memory)
		{
			UINT32 size = 0;
			memcpy(&size, memory, sizeof(UINT32));
			memory += sizeof(UINT32);

			memory = rttiReadElem(data.path, memory);
			memory = rttiReadElem(data.accessTimestamp, memory);

			return size;
		}

		static UINT32 getDynamicSize(const RecentProject& data)
		{
			UINT64 dataSize = sizeof(UINT32) + rttiGetElemSize(data.path) + rttiGetElemSize(data.accessTimestamp);

#if BS_DEBUG_MODE
			if (dataSize > std::numeric_limits<UINT32>::max())
			{
				__string_throwDataOverflowException();
			}
#endif

			return (UINT32)dataSize;
		}
	};

	/** @} */
	/** @endcond */
}