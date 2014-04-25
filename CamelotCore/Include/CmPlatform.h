#pragma once

#include "CmPrerequisites.h"

namespace BansheeEngine
{
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

	enum class OSMouseButton
	{
		Left, Middle, Right, Count
	};

	struct CM_EXPORT OSPointerButtonStates
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

	enum class OSDropType
	{
		FileList,
		None
	};

	class CM_EXPORT OSDropTarget
	{
	public:
		boost::signal<void(INT32 x, INT32 y)> onDragOver;
		boost::signal<void(INT32 x, INT32 y)> onDrop;
		boost::signal<void(INT32 x, INT32 y)> onEnter;
		boost::signal<void()> onLeave;

		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);
		OSDropType getDropType() const { return mDropType; }

		const Vector<WString>::type& getFileList() const { return *mFileList; }

		void _clear();
		void _setFileList(const Vector<WString>::type& fileList);
		void _setActive(bool active) { mActive = active; }

		bool _isInside(const Vector2I& pos) const;
		bool _isActive() const { return mActive; }
	private:
		friend class Platform;

		OSDropTarget(const RenderWindow* ownerWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		~OSDropTarget();

		const RenderWindow* getOwnerWindow() const { return mOwnerWindow; }
	private:
		INT32 mX, mY;
		UINT32 mWidth, mHeight;
		bool mActive;
		const RenderWindow* mOwnerWindow;

		OSDropType mDropType;

		union 
		{
			Vector<WString>::type* mFileList;
		};
	};
}

//Bring in the specific platform's header file
#if CM_PLATFORM == CM_PLATFORM_WIN32
# include "Win32/CmPlatformImpl.h"
#elif (CM_PLATFORM == CM_PLATFORM_LINUX)
# include "GLX/CmPlatformImpl.h"
#elif CM_PLATFORM == CM_PLATFORM_APPLE
# include "OSX/CmPlatformImpl.h"
#endif
