//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
#include <Image/BsPixelData.h>
#include <Image/BsPixelUtil.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <String/BsUnicode.h>
#include "BsUnixPlatform.h"
#include "BsUnixWindow.h"

namespace bs
{
	Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::onCursorMoved;
	Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::onCursorButtonPressed;
	Event<void(const Vector2I&, OSMouseButton button, const OSPointerButtonStates&)> Platform::onCursorButtonReleased;
	Event<void(const Vector2I&, const OSPointerButtonStates&)> Platform::onCursorDoubleClick;
	Event<void(InputCommandType)> Platform::onInputCommand;
	Event<void(float)> Platform::onMouseWheelScrolled;
	Event<void(UINT32)> Platform::onCharInput;

	Event<void(ct::RenderWindow*)> Platform::onMouseLeftWindow;
	Event<void()> Platform::onMouseCaptureChanged

	enum class X11CursorType
	{
		Arrow,
		ArrowDrag,
		ArrowLeftRight,
		Wait,
		IBeam,
		SizeTopLeft,
		SizeTopRight,
		SizeBotLeft,
		SizeBotRight,
		SizeLeft,
		SizeRight,
		SizeTop,
		SizeBottom,
		Deny,

		Count
	};;

	struct Platform::Pimpl
	{
		::Display* xDisplay = nullptr;
		::Window mainXWindow = 0;
		::Window fullscreenXWindow = 0;
		std::unordered_map<::Window, LinuxWindow*> windowMap;

		XIM IM;
		XIC IC;
		Time lastButtonPressTime;

		Atom atomDeleteWindow;

		// Clipboard
		WString clipboardData;

		// Cursor
		::Cursor currentCursor = None;
		::Cursor emptyCursor = None;
		bool isCursorHidden = false;

		Rect2I cursorClipRect;
		LinuxWindow* cursorClipWindow = nullptr;
		bool cursorClipEnabled = false;
	};

	static const UINT32 DOUBLE_CLICK_MS = 500;

	Platform::Pimpl* Platform::mData = bs_new<Platform::Pimpl>();

	Platform::~Platform() { }

	Vector2I Platform::getCursorPosition()
	{
		UINT32 screenCount = XScreenCount(mData->xDisplay);

		Vector2I pos;
		for(UINT32 i = 0; i < screenCount; ++i)
		{
			::Window outRoot, outChild;
			INT32 childX, childY;
			UINT32 mask;
			XQueryPointer(mData->xDisplay, XRootWindow(mData->xDisplay, i), &outRoot, &outChild, &pos.x,
					&pos.y, &childX, &childY, &mask);
		}

		return pos;
	}

	void Platform::setCursorPosition(const Vector2I& screenPos)
	{
		UINT32 screenCount = XScreenCount(mData->xDisplay);

		// Note assuming screens are laid out horizontally left to right
		INT32 screenX = 0;
		for(UINT32 i = 0; i < screenCount; ++i)
		{
			::Window root = XRootWindow(mData->xDisplay, i);
			INT32 screenXEnd = screenX + XDisplayWidth(mData->xDisplay, i);

			if(screenPos.x >= screenX && screenPos.x < screenXEnd)
			{
				XWarpPointer(mData->xDisplay, None, root, 0, 0, 0, 0, screenPos.x, screenPos.y);
				XFlush(mData->xDisplay);
				return;
			}

			screenX = screenXEnd;
		}
	}

	void Platform::captureMouse(const RenderWindow& window)
	{
		// TODOPORT
	}

	void Platform::releaseMouseCapture()
	{
		// TODOPORT
	}

	bool Platform::isPointOverWindow(const RenderWindow& window, const Vector2I& screenPos)
	{
		// TODOPORT
		return false;
	}

	void applyCurrentCursor(Platform::Pimpl* data, ::Window window)
	{
		if(data->isCursorHidden)
			XDefineCursor(data->xDisplay, window, data->emptyCursor);
		else
		{
			if (data->currentCursor != None)
				XDefineCursor(data->xDisplay, window, data->currentCursor);
			else
				XUndefineCursor(data->xDisplay, window);
		}
	}

	void updateClipBounds(Platform::Pimpl* data, LinuxWindow* window)
	{
		if(!data->cursorClipEnabled || data->cursorClipWindow != window)
			return;

		data->cursorClipRect.x = window->getLeft();
		data->cursorClipRect.y = window->getTop();
		data->cursorClipRect.width = window->getWidth();
		data->cursorClipRect.height = window->getHeight();
	}

	bool clipCursor(Platform::Pimpl* data, Vector2I& pos)
	{
		if(!data->cursorClipEnabled)
			return false;

		int32_t clippedX = pos.x - data->cursorClipRect.x;
		int32_t clippedY = pos.y - data->cursorClipRect.y;

		if(clippedX < 0)
			clippedX = data->cursorClipRect.x + data->cursorClipRect.width + clippedX;
		else if(clippedX >= data->cursorClipRect.width)
			clippedX = data->cursorClipRect.x + (clippedX - data->cursorClipRect.width);
		else
			clippedX = data->cursorClipRect.x + clippedX;

		if(clippedY < 0)
			clippedY = data->cursorClipRect.y + data->cursorClipRect.height + clippedY;
		else if(clippedY >= data->cursorClipRect.height)
			clippedY = data->cursorClipRect.y + (clippedY - data->cursorClipRect.height);
		else
			clippedY = data->cursorClipRect.y + clippedY;

		if(clippedX != pos.x || clippedY != pos.y)
		{
			pos.x = clippedX;
			pos.y = clippedY;

			return true;
		}

		return false;
	}

	void Platform::hideCursor()
	{
		mData->isCursorHidden = true;

		for(auto& entry : mData->windowMap)
			applyCurrentCursor(mData, entry.first);
	}

	void Platform::showCursor()
	{
		mData->isCursorHidden = false;

		for(auto& entry : mData->windowMap)
			applyCurrentCursor(mData, entry.first);
	}

	bool Platform::isCursorHidden()
	{
		return mData->isCursorHidden;
	}

	void Platform::clipCursorToWindow(const RenderWindow& window)
	{
		mData->cursorClipEnabled = true;
		mData->cursorClipWindow = window->_getInternal();

		updateClipBounds(window);

		Vector2I pos = getCursorPosition();

		if(clipCursor(mData, pos))
			setCursorPosition(pos);
	}

	void Platform::clipCursorToRect(const Rect2I& screenRect)
	{
		mData->cursorClipEnabled = true;
		mData->cursorClipRect = screenRect;
		mData->cursorClipWindow = nullptr;

		Vector2I pos = getCursorPosition();

		if(clipCursor(mData, pos))
			setCursorPosition(pos);
	}

	void Platform::clipCursorDisable()
	{
		mData->cursorClipEnabled = false;
		mData->cursorClipWindow = None;
	}

	void Platform::setCursor(PixelData& pixelData, const Vector2I& hotSpot)
	{
		// TODOPORT
	}

	void Platform::setIcon(const PixelData& pixelData)
	{
		SPtr<PixelData> resizedData = PixelData::create(32, 32, 1, PF_RGBA8);
		PixelUtil::scale(pixelData, *resizedData);

		// TODOPORT
	}

	void Platform::setCaptionNonClientAreas(const ct::RenderWindow& window, const Vector<Rect2I>& nonClientAreas)
	{
		// TODOPORT
	}

	void Platform::setResizeNonClientAreas(const ct::RenderWindow& window, const Vector<NonClientResizeArea>& nonClientAreas)
	{
		// Do nothing, resize areas not supported on Linux (but they are provided even on undecorated windows by the WM)
	}

	void Platform::resetNonClientAreas(const ct::RenderWindow& window)
	{
		// Do nothing, resize areas not supported on Linux (but they are provided even on undecorated windows by the WM)
	}

	void Platform::sleep(UINT32 duration)
	{
		usleep(duration * 1000);
	}

	OSDropTarget& Platform::createDropTarget(const RenderWindow* window, int x, int y, unsigned int width, unsigned int height)
	{
		// TODOPORT
	}

	void Platform::destroyDropTarget(OSDropTarget& target)
	{
		// TODOPORT
	}

	void Platform::copyToClipboard(const WString& string)
	{
		mData->clipboardData = string;

		Atom clipboardAtom = XInternAtom(mData->xDisplay, "CLIPBOARD", 0);
		XSetSelectionOwner(mData->xDisplay, clipboardAtom, mData->mainXWindow, CurrentTime);
	}

	WString Platform::copyFromClipboard()
	{
		Atom clipboardAtom = XInternAtom(mData->xDisplay, "CLIPBOARD", 0);
		::Window selOwner = XGetSelectionOwner(mData->xDisplay, clipboardAtom);

		if(selOwner == None)
			return L"";

		if(selOwner == mData->mainXWindow)
			return mData->clipboardData;

		XConvertSelection(mData->xDisplay, clipboardAtom, XA_STRING, clipboardAtom, mData->mainXWindow,
				CurrentTime);
		XFlush(mData->xDisplay);

		// Note: This might discard events if there are any in between the one we need. Ideally we let the
		// processEvents() handle them
		while(true)
		{
			XEvent event;
			XNextEvent(mData->xDisplay, &event);

			if(event.type == SelectionNotify && event.xselection.requestor == mData->mainXWindow)
				break;
		}

		Atom actualType;
		INT32 actualFormat;
		unsigned long length;
		unsigned long bytesRemaining;
		UINT8* data;
		XGetWindowProperty(mData->xDisplay, mData->mainXWindow, clipboardAtom,
				0, 0, False, AnyPropertyType, &actualType, &actualFormat, &length, &bytesRemaining, &data);

		if(bytesRemaining > 0)
		{
			unsigned long unused;
			INT32 result = XGetWindowProperty(mData->xDisplay, mData->mainXWindow, clipboardAtom,
					0, bytesRemaining, False, AnyPropertyType, &actualType, &actualFormat, &length,
					&unused, &data);

			if(result == Success)
				return UTF8::toWide(String((const char*)data));

			XFree(data);
		}

		return L"";
	}

	WString Platform::keyCodeToUnicode(UINT32 keyCode)
	{
		// TODOPORT
		return L"";
	}

	void Platform::_messagePump()
	{
		while(XPending(mData->xDisplay) > 0)
		{
			XEvent event;
			XNextEvent(mData->xDisplay, &event);

			switch (event.type)
			{
			case ClientMessage:
			{
				if(event.xclient.data.l[0] == mData->atomDeleteWindow)
					XDestroyWindow(mData->xDisplay, event.xclient.window);
			}
				break;
			case DestroyNotify:
			{
				auto iterFind = mData->windowMap.find(event.xdestroywindow.window);
				if (iterFind == mData->windowMap.end())
					break;

				LinuxWindow* window = iterFind->second;
				window->_cleanUp();

				if(mData->mainXWindow == 0)
					return;
			}
				break;
			case KeyPress:
			{
				// Process text input
				//// Check if input manager wants this event. If not, we process it.
				if(!XFilterEvent(&event, None))
				{
					Status status;
					uint8_t buffer[16];

					int32_t length = Xutf8LookupString(mData->IC, &event.xkey, (char*)buffer, sizeof(buffer), nullptr,
							&status);

					if(length > 0)
					{
						buffer[length] = '\0';

						// TODOPORT - Buffer now contains the UTF8 value of length 'length' bytes. I can consider converting
						// it to single UTF32 before returning
					}
				}

				// Process normal key press
				{
					static XComposeStatus keyboard;
					uint8_t buffer[16];
					KeySym symbol;
					XLookupString(&event.xkey, (char*)buffer, sizeof(buffer), &symbol, &keyboard);

					bool alt = event.xkey.state & Mod1Mask;
					bool control = event.xkey.state & ControlMask;
					bool shift = event.xkey.state & ShiftMask;

					// TODOPORT - Report key press
				}
			}
				break;
			case KeyRelease:
			{
				uint8_t buffer[16];
				KeySym symbol;
				XLookupString(&event.xkey, (char *) buffer, sizeof(buffer), &symbol, nullptr);

				bool alt = (event.xkey.state & Mod1Mask) != 0;
				bool control = (event.xkey.state & ControlMask) != 0;
				bool shift = (event.xkey.state & ShiftMask) != 0;

				// TODOPORT - Report key release
			}
				break;
			case ButtonPress:
			{
				uint32_t button = event.xbutton.button;
				switch(button)
				{
					// Button 4 & 5 is vertical wheel, 6 & 7 horizontal wheel, and we handle them elsewhere
				case Button1: // Left
				case Button2: // Middle
				case Button3: // Right
				case 8:
				case 9:
					int32_t x = event.xbutton.x_root;
					int32_t y = event.xbutton.y_root;

					bool alt = (event.xbutton.state & Mod1Mask) != 0;
					bool control = (event.xbutton.state & ControlMask) != 0;
					bool shift = (event.xbutton.state & ShiftMask) != 0;

					// TODOPORT - Report button press
					break;
				}

				// Handle double-click
				if(event.xbutton.time < (mData->lastButtonPressTime + DOUBLE_CLICK_MS))
				{
					// TODOPORT - Trigger double-click
					mData->lastButtonPressTime = 0;
				}
				else
					mData->lastButtonPressTime = event.xbutton.time;

				// Handle window dragging for windows without a title bar
				if(button == Button1)
				{
					auto iterFind = mData->windowMap.find(event.xbutton.window);
					if (iterFind != mData->windowMap.end())
					{
						LinuxWindow* window = iterFind->second;
						window->_dragStart(event.xbutton.x, event.xbutton.y);
					}
				}

				break;
			}
			case ButtonRelease:
			{
				uint32_t button = event.xbutton.button;
				bool alt = (event.xbutton.state & Mod1Mask) != 0;
				bool control = (event.xbutton.state & ControlMask) != 0;
				bool shift = (event.xbutton.state & ShiftMask) != 0;

				switch(button)
				{
				case Button2: // Middle
				case Button3: // Right
				case 8:
				case 9:
				{
					int32_t x = event.xbutton.x_root;
					int32_t y = event.xbutton.y_root;

					// TODOPORT - Report button release
					break;
				}
				case Button4: // Vertical mouse wheel
				case Button5:
				{
					int32_t delta = button == Button4 ? 1 : -1;

					// TODOPORT - Send mouse wheel scroll
				}
					break;
				case 6: // Horizontal mouse wheel
				case 7:
				{
					int32_t delta = button == 6 ? 1 : -1;

					// TODOPORT - Send mouse wheel scroll
				}
					break;
				}

				// Handle window dragging for windows without a title bar
				if(button == Button1)
				{
					auto iterFind = mData->windowMap.find(event.xbutton.window);
					if (iterFind != mData->windowMap.end())
					{
						LinuxWindow* window = iterFind->second;
						window->_dragEnd();
					}
				}

				break;
			}
			case MotionNotify:
			{
				Vector2I pos;
				pos.x = event.xmotion.x_root;
				pos.y = event.xmotion.y_root;

				// Handle clipping if enabled
				if(clipCursor(mData, pos))
					setCursorPosition(pos);

				// TODOPORT - Send mouse move event

				// Handle window dragging for windows without a title bar
				auto iterFind = mData->windowMap.find(event.xmotion.window);
				if (iterFind != mData->windowMap.end())
				{
					LinuxWindow* window = iterFind->second;
					window->_dragUpdate(event.xmotion.x, event.xmotion.y);
				}
			}
				break;
			case EnterNotify:
				if(event.xcrossing.mode == NotifyNormal)
				{
					// TODOPORT - Send mouse enter event
				}
				break;
			case LeaveNotify:
				if(event.xcrossing.mode == NotifyNormal)
				{
					Vector2I pos;
					pos.x = event.xcrossing.x_root;
					pos.y = event.xcrossing.y_root;

					if(clipCursor(mData, pos))
						setCursorPosition(pos);

					// TODOPORT - Send mouse leave event
				}
				break;
			case ConfigureNotify:
			{
				const XConfigureEvent &xce = event.xconfigure;

				auto iterFind = mData->windowMap.find(event.xdestroywindow.window);
				if (iterFind == mData->windowMap.end())
					break;

				LinuxWindow* window = iterFind->second;
				updateClipBounds(mData, window);

				// TODOPORT - Send move/resize event
			}
				break;
			case FocusIn:
				// Update input context focus
				XSetICFocus(mData->IC);
				break;
			case FocusOut:
				// Update input context focus
				XUnsetICFocus(mData->IC);
				break;
			case SelectionRequest:
			{
				// Send the data saved by the last clipboard copy operation
				Atom compoundTextAtom = XInternAtom(mData->xDisplay, "COMPOUND_TEXT", 0);
				Atom utf8StringAtom = XInternAtom(mData->xDisplay, "UTF8_STRING", 0);
				Atom targetsAtom = XInternAtom(mData->xDisplay, "TARGETS", 0);

				XSelectionRequestEvent& selReq = event.xselectionrequest;
				XEvent response;
				if(selReq.target == XA_STRING || selReq.target == compoundTextAtom || selReq.target == utf8StringAtom)
				{
					String utf8data = UTF8::fromWide(mData->clipboardData);

					const uint8_t* data = (const uint8_t*)utf8data.c_str();
					int32_t dataLength = utf8data.length();

					XChangeProperty(mData->xDisplay, selReq.requestor, selReq.property,
							selReq.target, 8, PropModeReplace, data, dataLength);

					response.xselection.property = selReq.property;
				}
				else if(selReq.target == targetsAtom)
				{
					Atom data[2];
					data[0] = utf8StringAtom;
					data[1] = XA_STRING;
					XChangeProperty (mData->xDisplay, selReq.requestor, selReq.property, selReq.target,
							8, PropModeReplace, (unsigned char*)&data, sizeof (data));

					response.xselection.property = selReq.property;
				}
				else
				{
					response.xselection.property = None;
				}

				response.xselection.type = SelectionNotify;
				response.xselection.display = selReq.display;
				response.xselection.requestor = selReq.requestor;
				response.xselection.selection = selReq.selection;
				response.xselection.target = selReq.target;
				response.xselection.time = selReq.time;

				XSendEvent (mData->xDisplay, selReq.requestor, 0, 0, &response);
				XFlush (mData->xDisplay);
			}
				break;
			default:
				break;
			}
		}
	}

	void Platform::_startUp()
	{
		// XInitThreads(); // TODO: Not sure if this will be necessary
		mData->xDisplay = XOpenDisplay(nullptr);

		if(XSupportsLocale())
		{
			XSetLocaleModifiers("");
			mData->IM = XOpenIM(mData->xDisplay, nullptr, nullptr, nullptr);

			// Note: Currently our windows don't support pre-edit and status areas, which are used for more complex types
			// of character input. Later on it might be beneficial to support them.
			mData->IC = XCreateIC(mData->IM, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, nullptr);
		}

		mData->atomDeleteWindow = XInternAtom(mData->xDisplay, "WM_DELETE_WINDOW", False);

		// Create empty cursor
		char data[1];
		memset(data, 0, sizeof(data));

		Pixmap pixmap = XCreateBitmapFromData(mData->xDisplay, DefaultRootWindow(mData->xDisplay), data, 1, 1);

		XColor color;
		color.red = color.green = color.blue = 0;
		mData->emptyCursor = XCreatePixmapCursor(mData->xDisplay, pixmap, pixmap, &color, &color, 0, 0);

		XFreePixmap(mData->xDisplay, pixmap);
	}

	void Platform::_update()
	{
		// Do nothing
	}

	void Platform::_coreUpdate()
	{
		_messagePump();
	}

	void Platform::_shutDown()
	{
		// Free empty cursor
		XFreeCursor(mData->xDisplay, mData->emptyCursor);
		mData->emptyCursor = None;

		if(mData->IC)
		{
			XDestroyIC(mData->IC);
			mData->IC = 0;
		}

		if(mData->IM)
		{
			XCloseIM(mData->IM);
			mData->IM = 0;
		}

		XCloseDisplay(mData->xDisplay);
		mData->xDisplay = nullptr;

		bs_delete(mData);
		mData = nullptr;
	}

	::Display* LinuxPlatform::getXDisplay()
	{
		return mData->xDisplay;
	}

	::Window LinuxPlatform::getMainXWindow()
	{
		return mData->mainXWindow;
	}

	void LinuxPlatform::_registerWindow(::Window xWindow, LinuxWindow* window)
	{
		// First window is assumed to be the main
		if(mData->mainXWindow == 0)
		{
			mData->mainXWindow = xWindow;

			// Input context client window must be set before use
			XSetICValues(mData->IC,
					XNClientWindow, xWindow,
					XNFocusWindow, xWindow,
					nullptr);
		}

		mData->windowMap[xWindow] = window;

		applyCurrentCursor(mData, xWindow);
	}

	void LinuxPlatform::_unregisterWindow(::Window xWindow)
	{
		auto iterFind = mData->windowMap.find(xWindow);
		if(iterFind != mData->windowMap.end())
		{
			if(mData->cursorClipEnabled && mData->cursorClipWindow == iterFind->second)
				clipCursorDisable();

			mData->windowMap.erase(iterFind);
		}

		if(mData->mainXWindow == xWindow)
			mData->mainXWindow = 0;
	}

	Pixmap LinuxPlatform::createPixmap(const SPtr<PixelData>& data)
	{
		SPtr<PixelData> bgraData = PixelData::create(data->getWidth(), data->getHeight(), 1, PF_BGRA8);
		PixelUtil::bulkPixelConversion(*data, *bgraData);

		uint32_t depth = XDefaultDepth(mData->xDisplay, 0);
		XImage* image = XCreateImage(mData->xDisplay, XDefaultVisual(mData->xDisplay, 0), depth, ZPixmap, 0,
				(char*)bgraData->getData(), data->getWidth(), data->getHeight(), 32, 0);

		Pixmap pixmap = XCreatePixmap(mData->xDisplay, XDefaultRootWindow(mData->xDisplay),
				data->getWidth(), data->getHeight(), depth);

		XGCValues gcValues;
		GC gc = XCreateGC(mData->xDisplay, pixmap, 0, &gcValues);
		XPutImage(mData->xDisplay, pixmap, gc, image, 0, 0, 0, 0, data->getWidth(), data->getHeight());
		XFreeGC(mData->xDisplay, gc);

		// Make sure XDestroyImage doesn't free the data pointed to by 'data.bytes'
		image->data = nullptr;
		XDestroyImage(image);

		return pixmap;
	}
}