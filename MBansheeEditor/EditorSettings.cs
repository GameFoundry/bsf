using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public static class EditorSettings
    {
        public static bool MoveHandleSnapActive
        {
            get { return Internal_GetMoveHandleSnapActive(); }
            set { Internal_SetMoveHandleSnapActive(value); }
        }

        public static bool RotateHandleSnapActive
        {
            get { return Internal_GetRotateHandleSnapActive(); }
            set { Internal_SetRotateHandleSnapActive(value); }
        }

        public static bool ScaleHandleSnapActive
        {
            get { return Internal_GetScaleHandleSnapActive(); }
            set { Internal_SetScaleHandleSnapActive(value); }
        }

        public static float MoveHandleSnapAmount
        {
            get { return Internal_GetMoveHandleSnapAmount(); }
            set { Internal_SetMoveHandleSnapAmount(value); }
        }

        public static float RotateHandleSnapAmount
        {
            get { return Internal_GetRotateHandleSnapAmount(); }
            set { Internal_SetRotateHandleSnapAmount(value); }
        }

        public static float ScaleHandleSnapAmount
        {
            get { return Internal_GetScaleHandleSnapAmount(); }
            set { Internal_SetScaleHandleSnapAmount(value); }
        }

        public static float DefaultHandleSize
        {
            get { return Internal_GetDefaultHandleSize(); }
            set { Internal_SetDefaultHandleSize(value); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetMoveHandleSnapActive();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMoveHandleSnapActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetRotateHandleSnapActive();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotateHandleSnapActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_GetScaleHandleSnapActive();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetScaleHandleSnapActive(bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetMoveHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMoveHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRotateHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotateHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetScaleHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetScaleHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetDefaultHandleSize();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDefaultHandleSize(float value);
    }
}
