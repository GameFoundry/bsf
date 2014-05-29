#pragma once

#include "BsCorePrerequisites.h"

namespace BansheeEngine
{
	/**
	 * @brief	Contains values representing default
	 *			mouse cursor types.
	 */
	enum class PlatformCursorType
	{
		Arrow,
		Wait,
		IBeam,
		Help,
		Hand,
		SizeAll,
		SizeNESW,
		SizeNS,
		SizeNWSE,
		SizeWE
	};

	/**
	 * @brief	Contains values reprenting window non client areas.
	 *
	 * @note	These are used for things like resize/move and tell the OS
	 *			where each of those areas are on our window.
	 */
	enum class NonClientAreaBorderType
	{
		TopLeft,
		Top,
		TopRight,
		Left,
		Right,
		BottomLeft,
		Bottom,
		BottomRight	
	};

	/**
	 * @brief	Types of mouse buttons provided by the OS.
	 */
	enum class OSMouseButton
	{
		Left, Middle, Right, Count
	};

	/**
	 * @brief	Describes pointer (mouse, touch) states as reported
	 *			by the OS.
	 */
	struct BS_CORE_EXPORT OSPointerButtonStates
	{
		OSPointerButtonStates()
		{
			mouseButtons[0] = false;
			mouseButtons[1] = false;
			mouseButtons[2] = false;

			shift = false;
			ctrl = false;
		}

		bool mouseButtons[OSMouseButton::Count];
		bool shift, ctrl;
	};

	/**
	 * @brief	Type of drop event type. This is used
	 *			when dragging items over drop targets.
	 */
	enum class OSDropType
	{
		FileList,
		None
	};

	/**
	 * @brief	Drop targets allow you to register a certain portion of a window as a drop target that
	 *			accepts certain drop types. Accepted drop types are provided by the OS and include things
	 *			like file and item dragging.
	 *
	 *			You will receive events with the specified drop area as long as it is active.
	 */
	class BS_CORE_EXPORT OSDropTarget
	{
	public:
		/**
		 * @brief	Triggered when a pointer is being dragged over the drop area.
		 *			Provides window coordinates of the pointer position.
		 */
		Event<void(INT32 x, INT32 y)> onDragOver;

		/**
		 * @brief	Triggered when the user completes a drop while pointer is over
		 *			the drop area.
		 *			Provides window coordinates of the pointer position.
		 */
		Event<void(INT32 x, INT32 y)> onDrop;

		/**
		 * @brief	Triggered when a pointer enters the drop area.
		 *			Provides window coordinates of the pointer position.
		 */
		Event<void(INT32 x, INT32 y)> onEnter;

		/**
		 * @brief	Triggered when a pointer leaves the drop area.
		 */
		Event<void()> onLeave;

		/**
		 * @brief	Sets the drop target area, in local window coordinates.
		 */
		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);

		/**
		 * @brief	Gets the type of drop that this drop target is looking for. Only
		 *			valid after a drop has been triggered.
		 */
		OSDropType getDropType() const { return mDropType; }

		/**
		 * @brief	Returns a list of files received by the drop target. Only valid
		 *			after a drop of FileList type has been triggered.
		 */
		const Vector<WString>& getFileList() const { return *mFileList; }

		/**
		 * @brief	Internal method. Clears all internal values.
		 */
		void _clear();

		/**
		 * @brief	Internal method. Sets the file list and marks the drop event as FileList.
		 */
		void _setFileList(const Vector<WString>& fileList);

		/**
		 * @brief	Marks the drop area as inactive or active.
		 */
		void _setActive(bool active) { mActive = active; }

		/**
		 * @brief	Checks is the specified position within the current drop area.
		 *			Position should be in window local coordinates.
		 */
		bool _isInside(const Vector2I& pos) const;

		/**
		 * @brief	Returns true if the drop target is active.
		 */
		bool _isActive() const { return mActive; }
	private:
		friend class Platform;

		OSDropTarget(const RenderWindow* ownerWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		~OSDropTarget();

		/**
		 * @brief	Returns a render window this drop target is attached to.
		 */
		const RenderWindow* getOwnerWindow() const { return mOwnerWindow; }
	private:
		INT32 mX, mY;
		UINT32 mWidth, mHeight;
		bool mActive;
		const RenderWindow* mOwnerWindow;

		OSDropType mDropType;

		union 
		{
			Vector<WString>* mFileList;
		};
	};
}

// Bring in the specific platform's header file
#if BS_PLATFORM == BS_PLATFORM_WIN32
# include "Win32/BsPlatformImpl.h"
#elif (BS_PLATFORM == BS_PLATFORM_LINUX)
# include "GLX/BsPlatformImpl.h"
#elif BS_PLATFORM == BS_PLATFORM_APPLE
# include "OSX/BsPlatformImpl.h"
#endif
