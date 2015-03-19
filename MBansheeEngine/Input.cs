using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
	public struct ButtonEvent
	{
		internal ButtonCode buttonCode;
        internal int deviceIdx;

	    internal ButtonEvent(ButtonCode buttonCode, int deviceIdx)
	    {
	        this.buttonCode = buttonCode;
	        this.deviceIdx = deviceIdx;
	    }

        public ButtonCode Button { get { return buttonCode; } }
        public int DeviceIndex { get { return deviceIdx; } }

		public bool IsKeyboard { get { return ((int)buttonCode & 0xC0000000) == 0; }}
        public bool IsMouse { get { return ((int)buttonCode & 0x80000000) != 0; } }
        public bool IsGamepad { get { return ((int)buttonCode & 0x40000000) != 0; } }
	};

    // Do not change IDs, must match PointerEventButton C++ enum
	public enum PointerButton
	{
		Left, Middle, Right, Count
	};

    public struct PointerEvent
    {
        internal Vector2I _screenPos;
        internal Vector2I _delta;
        internal PointerButton _button;

        internal bool _shift;
        internal bool _control;
        internal bool _alt;

        internal float _mouseWheelScrollAmount;

        internal PointerEvent(Vector2I screenPos, Vector2I delta, PointerButton button,
            bool shift, bool control, bool alt, float mouseWheelScrollAmount)
        {
            _screenPos = screenPos;
            _delta = delta;
            _button = button;

            _shift = shift;
            _control = control;
            _alt = alt;

            _mouseWheelScrollAmount = mouseWheelScrollAmount;
        }

        public Vector2I screenPos { get { return _screenPos; } }
        public Vector2I delta { get { return _delta; } }
        public PointerButton button { get { return _button; } }

        public bool shift { get { return _shift; } }
        public bool control { get { return _control; } }
        public bool alt { get { return _alt; } }

        public float scrollAmount { get { return _mouseWheelScrollAmount; } }
    }

    public struct TextInputEvent
    {
        internal int textChar;

        internal TextInputEvent(int textChar)
        {
            this.textChar = textChar;
        }

        public int Char { get { return textChar; } }
    }

    public static class Input
    {
        public delegate void ButtonEventDelegate(ButtonEvent ev);
        public delegate void TextInputEventDelegate(TextInputEvent ev);
        public delegate void PointerEventDelegate(PointerEvent ev);

		public static event ButtonEventDelegate OnButtonDown;
        public static event ButtonEventDelegate OnButtonUp;
        public static event TextInputEventDelegate OnCharInput;
        public static event PointerEventDelegate OnPointerMoved;
        public static event PointerEventDelegate OnPointerPressed;
        public static event PointerEventDelegate OnPointerReleased;
        public static event PointerEventDelegate OnPointerDoubleClick;

        public static float GetAxisValue(InputAxis axis, int deviceIdx = 0)
        {
            return Internal_GetAxisValue(axis, deviceIdx);
        }

        public static bool IsButtonHeld(ButtonCode code, int deviceIdx = 0)
        {
            return Internal_IsButtonHeld(code, deviceIdx);
        }

        public static bool IsButtonUp(ButtonCode code, int deviceIdx = 0)
        {
            return Internal_IsButtonUp(code, deviceIdx);
        }

        public static bool IsButtonDown(ButtonCode code, int deviceIdx = 0)
        {
            return Internal_IsButtonDown(code, deviceIdx);
        }

        public static bool IsPointerButtonHeld(PointerButton code)
        {
            return Internal_IsPointerButtonHeld(code);
        }

        public static bool IsPointerButtonUp(PointerButton code)
        {
            return Internal_IsPointerButtonUp(code);
        }

        public static bool IsPointerButtonDown(PointerButton code)
        {
            return Internal_IsPointerButtonDown(code);
        }

        public static bool IsPointerDoubleClicked()
        {
            return Internal_IsPointerDoubleClicked();
        }

        public static Vector2I PointerPosition
        {
            get
            {
                Vector2I value;
                Internal_GetPointerPosition(out value);
                return value;
            }
        }

        public static Vector2I PointerDelta
        {
            get
            {
                Vector2I value;
                Internal_GetPointerDelta(out value);
                return value;
            }
        }

        private static void Internal_TriggerButtonDown(ButtonCode code, int deviceIdx)
        {
            ButtonEvent ev = new ButtonEvent(code, deviceIdx);

            if (OnButtonDown != null)
                OnButtonDown(ev);
        }

        private static void Internal_TriggerButtonUp(ButtonCode code, int deviceIdx)
        {
            ButtonEvent ev = new ButtonEvent(code, deviceIdx);

            if (OnButtonUp != null)
                OnButtonUp(ev);
        }

        private static void Internal_TriggerCharInput(int textChar)
        {
            TextInputEvent ev = new TextInputEvent(textChar);

            if (OnCharInput != null)
                OnCharInput(ev);
        }

        private static void Internal_TriggerPointerMove(Vector2I screenPos, Vector2I delta, PointerButton button, bool shift, 
            bool ctrl, bool alt, float scrollAmount)
        {
            PointerEvent ev = new PointerEvent(screenPos, delta, button, shift, ctrl, alt, scrollAmount);

            if (OnPointerMoved != null)
                OnPointerMoved(ev);
        }

        private static void Internal_TriggerPointerPressed(Vector2I screenPos, Vector2I delta, PointerButton button, bool shift,
            bool ctrl, bool alt, float scrollAmount)
        {
            PointerEvent ev = new PointerEvent(screenPos, delta, button, shift, ctrl, alt, scrollAmount);

            if (OnPointerPressed != null)
                OnPointerPressed(ev);
        }

        private static void Internal_TriggerPointerReleased(Vector2I screenPos, Vector2I delta, PointerButton button, bool shift,
            bool ctrl, bool alt, float scrollAmount)
        {
            PointerEvent ev = new PointerEvent(screenPos, delta, button, shift, ctrl, alt, scrollAmount);

            if (OnPointerReleased != null)
                OnPointerReleased(ev);
        }

        private static void Internal_TriggerPointerDoubleClick(Vector2I screenPos, Vector2I delta, PointerButton button, bool shift,
            bool ctrl, bool alt, float scrollAmount)
        {
            PointerEvent ev = new PointerEvent(screenPos, delta, button, shift, ctrl, alt, scrollAmount);

            if (OnPointerDoubleClick != null)
                OnPointerDoubleClick(ev);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetAxisValue(InputAxis axis, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsButtonHeld(ButtonCode keyCode, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsButtonUp(ButtonCode keyCode, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsButtonDown(ButtonCode keyCode, int deviceIdx);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsPointerDoubleClicked();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsPointerButtonHeld(PointerButton keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsPointerButtonUp(PointerButton keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsPointerButtonDown(PointerButton keyCode);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPointerPosition(out Vector2I position);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetPointerDelta(out Vector2I delta);
    }

    // Do not change IDs, must match ButtonCode C++ enum
    public enum ButtonCode : uint
    {
        Unassigned = 0x00,
        Escape = 0x01,
        Num1 = 0x02,
        Num2 = 0x03,
        Num3 = 0x04,
        Num4 = 0x05,
        Num5 = 0x06,
        Num6 = 0x07,
        Num7 = 0x08,
        Num8 = 0x09,
        Num9 = 0x0A,
        Num0 = 0x0B,
        Minus = 0x0C, 
        Equals = 0x0D,
        Back = 0x0E,
        Tab = 0x0F,
        Q = 0x10,
        W = 0x11,
        E = 0x12,
        R = 0x13,
        T = 0x14,
        Y = 0x15,
        U = 0x16,
        I = 0x17,
        O = 0x18,
        P = 0x19,
        LeftBracket = 0x1A,
        RightBracket = 0x1B,
        Return = 0x1C,
        LeftControl = 0x1D,
        A = 0x1E,
        S = 0x1F,
        D = 0x20,
        F = 0x21,
        G = 0x22,
        H = 0x23,
        J = 0x24,
        K = 0x25,
        L = 0x26,
        Semicolon = 0x27,
        Apostrophe = 0x28,
        Grave = 0x29,
        LeftShift = 0x2A,
        Backslash = 0x2B,
        Z = 0x2C,
        X = 0x2D,
        C = 0x2E,
        V = 0x2F,
        B = 0x30,
        N = 0x31,
        M = 0x32,
        Comma = 0x33,
        Period = 0x34, 
        Slash = 0x35,
        RightShift = 0x36,
        KeypadMultiply = 0x37,
        LeftMenu = 0x38,
        Space = 0x39,
        CapsLock = 0x3A,
        F1 = 0x3B,
        F2 = 0x3C,
        F3 = 0x3D,
        F4 = 0x3E,
        F5 = 0x3F,
        F6 = 0x40,
        F7 = 0x41,
        F8 = 0x42,
        F9 = 0x43,
        F10 = 0x44,
        NumLock = 0x45,
        ScrollLock = 0x46,
        Keypad7 = 0x47,
        Keypad8 = 0x48,
        Keypad9 = 0x49,
        KeypadSubtract = 0x4A,
        Keypad4 = 0x4B,
        Keypad5 = 0x4C,
        Keypad6 = 0x4D,
        KeypadAdd = 0x4E,
        Keypad1 = 0x4F,
        Keypad2 = 0x50,
        Keypad3 = 0x51,
        Keypad0 = 0x52,
        KeypadDecimal = 0x53,
        F11 = 0x57,
        F12 = 0x58,
        F13 = 0x64,
        F14 = 0x65,
        F15 = 0x66,
        KeypadEquals = 0x8D,
        At = 0x91,
        Colon = 0x92,
        NumpadEnter = 0x9C,
        RightControl = 0x9D,
        KeypadComma = 0xB3,
        KeypadDivide = 0xB5,
        RightMenu = 0xB8,
        Pause = 0xC5,
        Home = 0xC7,
        Up = 0xC8,
        PageUp = 0xC9,
        Left = 0xCB,
        Right = 0xCD,
        End = 0xCF,
        Down = 0xD0,
        PageDown = 0xD1,
        Insert = 0xD2,
        Delete = 0xD3,
        LeftWindows = 0xDB,
        RightWindows = 0xDC,
        MouseLeft = 0x800000EE,
        MouseRight = 0x800000EF,
        MouseMiddle = 0x800000F0,
        MouseBtn4 = 0x800000F1,
        MouseBtn5 = 0x800000F2,
        MouseBtn6 = 0x800000F3,
        MouseBtn7 = 0x800000F4,
        MouseBtn8 = 0x800000F5,
        MouseBtn9 = 0x800000F6,
        MouseBtn10 = 0x800000F7,
        MouseBtn11 = 0x800000F8,
        MouseBtn12 = 0x800000F9,
        MouseBtn13 = 0x800000FA,
        MouseBtn14 = 0x800000FB,
        MouseBtn15 = 0x800000FC,
        MouseBtn16 = 0x800000FD,
        GamepadA = 0x4000010F,
        GamepadB = 0x40000110,
        GamepadX = 0x40000111,
        GamepadY = 0x40000112,
        GamepadLB = 0x40000113,
        GamepadRB = 0x40000114,
        GamepadLS = 0x40000115,
        GamepadRS = 0x40000116,
        GamepadBack = 0x40000117,
        GamepadStart = 0x40000118,
        GamepadDPadLeft = 0x40000119,
        GamepadDPadRight = 0x4000011A,
        GamepadDPadUp = 0x4000011B,
        GamepadDPatDown = 0x4000011C,
        GamepadBtn1 = 0x4000011D,
        GamepadBtn2 = 0x4000011E,
        GamepadBtn3 = 0x4000011F,
        GamepadBtn4 = 0x40000120,
        GamepadBtn5 = 0x40000121,
        GamepadBtn6 = 0x40000122,
        GamepadBtn7 = 0x40000123,
        GamepadBtn8 = 0x40000124,
        GamepadBtn9 = 0x40000125,
        GamepadBtn10 = 0x40000126,
        GamepadBtn11 = 0x40000127,
        GamepadBtn12 = 0x40000128,
        GamepadBtn13 = 0x40000129,
        GamepadBtn14 = 0x4000012A,
        GamepadBtn15 = 0x4000012B,
        GamepadBtn16 = 0x4000012C,
        Count = 249,
        NumKeys = 203, // IMPORTANT: Make sure to update these if you modify the values above
        NumMouseButtons = 16,
        NumGamepadButtons = 30,
    };

    // Do not change IDs, must match InputAxis C++ enum
	public enum InputAxis
	{
		MouseX,
		MouseY,
		MouseZ,
		LeftStickX,
		LeftStickY,
		RightStickX,
		RightStickY,
		LeftTrigger,
		RightTrigger,
		Count // Keep at end
	};
}
