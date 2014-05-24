#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Represents a video mode usable in full-screen windows.
	 */
	class CM_EXPORT VideoMode
	{
	public:
		VideoMode(UINT32 width, UINT32 height, VideoOutputInfo* parentOutputInfo);
		virtual ~VideoMode();

		VideoMode(const VideoMode&) = delete; // Make non-copyable
		VideoMode& operator=(const VideoMode&) = delete; // Make non-copyable

		/**
		 * @brief	Width of the front/back buffer in pixels.
		 */
		UINT32 getWidth() const { return mWidth; }

		/**
		 * @brief	Height of the front/back buffer in pixels.
		 */
		UINT32 getHeight() const { return mHeight; }

		/**
		 * @brief	Returns the number of available refresh rates for this mode.
		 */
		UINT32 getNumRefreshRates() const { return (UINT32)mRefreshRates.size(); }

		/**
		 * @brief	Returns a refresh rate in Hertz for the specified index.
		 */
		virtual float getRefreshRate(UINT32 idx) const { return mRefreshRates.at(idx); }

		/**
		 * @brief	Returns information about the parent output.
		 */
		const VideoOutputInfo& getParentOutput() const { return *mParentOutputInfo; }
	protected:
		UINT32 mWidth = 1280;
		UINT32 mHeight = 720;
		VideoOutputInfo* mParentOutputInfo = nullptr;

		Vector<float> mRefreshRates;
	};

	/**
	 * @brief	Contains information about a video output device, including
	 *			a list of all available video modes.
	 */
	class CM_EXPORT VideoOutputInfo
	{
	public:
		VideoOutputInfo() { }
		virtual ~VideoOutputInfo();

		VideoOutputInfo(const VideoOutputInfo&) = delete; // Make non-copyable
		VideoOutputInfo& operator=(const VideoOutputInfo&) = delete; // Make non-copyable

		/**
		 * @brief	Name of the output device.
		 */
		const String& getName() const { return mName; }

		/**
		 * @brief	Number of available video modes for this output.
		 */
		UINT32 getNumVideoModes() const { return (UINT32)mVideoModes.size(); }

		/**
		 * @brief	Returns video mode at the specified index.
		 */
		const VideoMode& getVideoMode(UINT32 idx) const { return *mVideoModes.at(idx); }

		/**
		 * @brief	Returns the video mode currently used by the desktop.
		 */
		const VideoMode& getDesktopVideoMode() const { return *mDesktopVideoMode; }

	protected:
		String mName;
		Vector<VideoMode*> mVideoModes;
		VideoMode* mDesktopVideoMode = nullptr;
	};

	/**
	 * @brief	Contains information about available output devices (e.g. monitor) 
	 *			and their video modes.
	 */
	class CM_EXPORT VideoModeInfo
	{
	public:
		VideoModeInfo() { }
		virtual ~VideoModeInfo();

		VideoModeInfo(const VideoModeInfo&) = delete; // Make non-copyable
		VideoModeInfo& operator=(const VideoModeInfo&) = delete; // Make non-copyable

		/**
		 * @brief	Returns the number of available output devices.
		 */
		UINT32 getNumOutputs() const { return (UINT32)mOutputs.size(); }

		/**
		 * @brief	Returns video mode information about a specific output device.
		 */
		const VideoOutputInfo& getOutputInfo(UINT32 idx) const { return *mOutputs[idx]; }

	protected:
		Vector<VideoOutputInfo*> mOutputs;
	};
}