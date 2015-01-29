using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal static class EditorSettings
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

        public static float MoveHandleSnapAmount
        {
            get { return Internal_GetMoveHandleSnapAmount(); }
            set { Internal_SetMoveHandleSnapAmount(value); }
        }

        public static Degree RotateHandleSnapAmount
        {
            get { return Internal_GetRotateHandleSnapAmount(); }
            set { Internal_SetRotateHandleSnapAmount(value.GetDegrees()); }
        }

        public static float DefaultHandleSize
        {
            get { return Internal_GetDefaultHandleSize(); }
            set { Internal_SetDefaultHandleSize(value); }
        }

        public static SceneViewTool ActiveSceneTool
        {
            get { return (SceneViewTool)Internal_GetActiveSceneTool(); }
            set { Internal_SetActiveSceneTool((int)value); }
        }

        public static HandleCoordinateMode ActiveCoordinateMode
        {
            get { return (HandleCoordinateMode)Internal_GetActiveCoordinateMode(); }
            set { Internal_SetActiveCoordinateMode((int)value); }
        }

        public static HandlePivotMode ActivePivotMode
        {
            get { return (HandlePivotMode)Internal_GetActivePivotMode(); }
            set { Internal_SetActivePivotMode((int)value); }
        }

        public static int Hash
        {
            get { return Internal_GetHash(); }
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
        private static extern float Internal_GetMoveHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetMoveHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetRotateHandleSnapAmount();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetRotateHandleSnapAmount(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetDefaultHandleSize();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetDefaultHandleSize(float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetActiveSceneTool();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActiveSceneTool(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetActiveCoordinateMode();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActiveCoordinateMode(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetActivePivotMode();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetActivePivotMode(int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetHash();
    }
}
