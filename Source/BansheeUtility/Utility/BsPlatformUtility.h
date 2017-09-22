//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#pragma once

#include "Prerequisites/BsPlatformDefines.h"
#include "String/BsString.h"
#include "Prerequisites/BsTypes.h"

namespace bs
{
	/** @addtogroup General
	 *  @{
	 */

	struct MACAddress;

	/** Possible type of platform file dialogs. */
	enum class FileDialogType
	{
		OpenFile = 0x0, 
		OpenFolder = 0x1, 
		Save = 0x2,
		Multiselect = 0x10000,
		TypeMask = 0xFFFF
	};

	/** Contains information about available GPUs on the system. */
	struct GPUInfo
	{
		String names[5];
		UINT32 numGPUs;
	};

	/** Contains information about the system hardware and operating system. */
	struct SystemInfo
	{
		String cpuManufacturer;
		String cpuModel;
		UINT32 cpuClockSpeedMhz;
		UINT32 cpuNumCores;
		UINT32 memoryAmountMb;
		String osName;
		bool osIs64Bit;

		GPUInfo gpuInfo;
	};

	/** Provides access to various operating system specific utility functions. */
	class BS_UTILITY_EXPORT PlatformUtility
	{
	public:
		/**
		 * Terminates the current process.
		 * 			
		 * @param[in]	force	True if the process should be forcefully terminated with no cleanup.
		 */
		static void terminate(bool force = false);

		/** Returns information about the underlying hardware. */
		static SystemInfo getSystemInfo();

		/** Creates a new universally unique identifier and returns it as a string. */
		static String generateUUID();

		/**
		 * Opens the provided file or folder using the default application for that file type, as specified by the operating 
		 * system.
		 *
		 * @param[in]	path	Absolute path to the file or folder to open.
		 */
		static void open(const Path& path);

		/** @name Internal
		 *  @{
		 */

		/** 
		 * Assigns information about GPU hardware. This data will be returned by getSystemInfo() when requested. This is
		 * expeced to be called by the render API backend when initialized.
		 */
		static void _setGPUInfo(GPUInfo gpuInfo) { sGPUInfo = gpuInfo; }
		/** @} */

	private:
		static GPUInfo sGPUInfo;
	};

	/** @} */
}
