using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;

namespace BansheeEngine
{
    public static class Cursor
    {
        public static Vector2I ScreenPosition
        {
            get
            {
                Vector2I value;
                Internal_GetScreenPosition(out value);
                return value;
            }

            set
            {
                Internal_SetScreenPosition(value);
            }
        }

        public static void Hide()
        {
            Internal_Hide();
        }

        public static void Show()
        {
            Internal_Show();
        }

        public static void ClipToRect(Rect2I area)
        {
            Internal_ClipToRect(area);
        }

        public static void ClipDisable()
        {
            Internal_ClipDisable();
        }

        public static void SetCursor(string name)
        {
            Internal_SetCursorStr(name);
        }

        public static void SetCursor(CursorType type)
        {
            Internal_SetCursor(type);
        }

        public static void SetCursorIcon(string name, PixelData iconData, Vector2I hotspot)
        {
            Internal_SetCursorIconStr(name, iconData, hotspot);
        }

        public static void SetCursorIcon(CursorType type, PixelData iconData, Vector2I hotspot)
        {
            Internal_SetCursorIcon(type, iconData, hotspot);
        }

        public static void ClearCursorIcon(string name)
        {
            Internal_ClearCursorIconStr(name);
        }

        public static void ClearCursorIcon(CursorType type)
        {
            Internal_ClearCursorIcon(type);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetScreenPosition(out Vector2I value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetScreenPosition(Vector2I value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Hide();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Show();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClipToRect(Rect2I value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClipDisable();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCursorStr(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCursor(CursorType cursor);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCursorIconStr(string name, PixelData iconData, Vector2I hotspot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCursorIcon(CursorType cursor, PixelData iconData, Vector2I hotspot);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearCursorIconStr(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearCursorIcon(CursorType cursor);
    }

    // Note: Do not modify indexes, they need to match C++ enum CursorType
	public enum CursorType
	{
		Arrow,
		ArrowDrag,
		ArrowLeftRight,
		Wait,
		IBeam,
		SizeNESW,
		SizeNS,
		SizeNWSE,
		SizeWE,
		Deny,

		// Keep at the end
		Count
	};
}
