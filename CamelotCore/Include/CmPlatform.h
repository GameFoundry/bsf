#pragma once

#include "CmPrerequisites.h"

namespace CamelotFramework
{
	enum class CursorType
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

	struct CM_EXPORT OSPositionalInputButtonStates
	{
		OSPositionalInputButtonStates()
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

		void setArea(INT32 x, INT32 y, UINT32 width, UINT32 height);
		OSDropType getDropType() const { return mDropType; }

		const Vector<WString>::type& getFileList() const { return *mFileList; }
	private:
		friend class Platform;

		OSDropTarget(const RenderWindow* ownerWindow, INT32 x, INT32 y, UINT32 width, UINT32 height);
		~OSDropTarget();

		void clean();
		void setFileList(const Vector<WString>::type& fileList);
		const RenderWindow* getOwnerWindow() const { return mOwnerWindow; }
	private:
		INT32 mX, mY;
		UINT32 mWidth, mHeight;
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
