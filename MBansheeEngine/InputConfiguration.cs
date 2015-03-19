using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    public sealed class InputConfiguration : ScriptObject
    {
        public InputConfiguration()
        {
            Internal_CreateInstance(this);
        }

        public UInt64 RepeatInternal
        {
            get
            {
                return Internal_GetRepeatInterval(mCachedPtr);
            }

            set
            {
                Internal_SetRepeatInterval(mCachedPtr, value);
            }
        }

        public void RegisterButton(String name, ButtonCode buttonCode, 
            ButtonModifier modifiers = ButtonModifier.None, bool repeatable = false)
        {
            Internal_RegisterButton(mCachedPtr, name, buttonCode, modifiers, repeatable);
        }

        public void UnregisterButton(String name)
        {
            Internal_UnregisterButton(mCachedPtr, name);
        }

        public void RegisterAxis(String name, InputAxis type, float deadZone = 0.0001f, 
            float sensitivity = 1.0f, bool invert = false)
        {
            Internal_RegisterAxis(mCachedPtr, name, type, deadZone, sensitivity, invert);
        }

        public void UnregisterAxis(String name)
        {
            Internal_UnregisterAxis(mCachedPtr, name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(InputConfiguration inputConfig);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RegisterButton(IntPtr thisPtr, String name, ButtonCode buttonCode,
            ButtonModifier modifiers, bool repeatable);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnregisterButton(IntPtr thisPtr, String name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_RegisterAxis(IntPtr thisPtr, String name, InputAxis type, float deadZone,
            float sensitivity, bool invert);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UnregisterAxis(IntPtr thisPtr, String name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRepeatInterval(IntPtr thisPtr, UInt64 milliseconds);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern UInt64 Internal_GetRepeatInterval(IntPtr thisPtr);
    }

    // Do not modify IDs, they must match the C++ enum VButtonModifier
    public enum ButtonModifier
    {
        None = 0x00,
        Shift = 0x01,
        Ctrl = 0x02,
        Alt = 0x04,
        ShiftCtrl = 0x03,
        CtrlAlt = 0x06,
        ShiftAlt = 0x05,
        ShiftCtrlAlt = 0x07
    };

    [StructLayout(LayoutKind.Sequential)]
    public struct VirtualButton
    {
        private readonly int buttonId;

        public VirtualButton(string name)
        {
            buttonId = Internal_InitVirtualButton(name);
        }

        public static bool operator ==(VirtualButton lhs, VirtualButton rhs)
        {
            return lhs.buttonId == rhs.buttonId;
        }

        public static bool operator !=(VirtualButton lhs, VirtualButton rhs)
        {
            return !(lhs == rhs);
        }

        public override int GetHashCode()
        {
            return buttonId.GetHashCode();
        }

        public override bool Equals(object other)
        {
            if (!(other is VirtualButton))
                return false;

            VirtualButton otherBtn = (VirtualButton)other;
            if (buttonId.Equals(otherBtn.buttonId))
                return true;

            return false;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_InitVirtualButton(String name);
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct VirtualAxis
    {
        private readonly int axisId;

        public VirtualAxis(string name)
        {
            axisId = Internal_InitVirtualAxis(name);
        }

        public static bool operator ==(VirtualAxis lhs, VirtualAxis rhs)
        {
            return lhs.axisId == rhs.axisId;
        }

        public static bool operator !=(VirtualAxis lhs, VirtualAxis rhs)
        {
            return !(lhs == rhs);
        }

        public override int GetHashCode()
        {
            return axisId.GetHashCode();
        }

        public override bool Equals(object other)
        {
            if (!(other is VirtualAxis))
                return false;

            VirtualAxis otherAxis = (VirtualAxis)other;
            if (axisId.Equals(otherAxis.axisId))
                return true;

            return false;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_InitVirtualAxis(String name);
    }
}
