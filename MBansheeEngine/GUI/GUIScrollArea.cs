using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    public enum ScrollBarType
	{
		ShowIfDoesntFit,
		AlwaysShow,
		NeverShow
	};

    public sealed class GUIScrollArea : GUIElement
    {
        private GUILayout _mainLayout;

        public GUILayout Layout
        {
            get { return _mainLayout; }
        }

        public float HorizontalScroll
        {
            get { return Internal_GetHorzScroll(mCachedPtr); }
            set { Internal_SetHorzScroll(mCachedPtr, value); }
        }

        public float VerticalScroll
        {
            get { return Internal_GetVertScroll(mCachedPtr); }
            set { Internal_SetVertScroll(mCachedPtr, value); }
        }

        public Rect2I ContentBounds
        {
            get { return Internal_GetContentBounds(mCachedPtr); }
        }

        public int ScrollBarWidth
        {
            get { return Internal_GetScrollBarWidth(mCachedPtr); }
        }

        public GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, string scrollBarStyle,
            string scrollAreaStyle, params GUIOption[] options)
        {
            Internal_CreateInstance(this, vertBarType, horzBarType, scrollBarStyle, scrollAreaStyle, options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, vertBarType, horzBarType, "", style, options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(ScrollBarType vertBarType, ScrollBarType horzBarType, params GUIOption[] options)
        {
            Internal_CreateInstance(this, vertBarType, horzBarType, "", "", options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, "", style, options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(params GUIOption[] options)
        {
            Internal_CreateInstance(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, "", "", options);
            _mainLayout = new GUILayoutY(this);
        }

        public GUIScrollArea(string scrollBarStyle, string scrollAreaStyle, params GUIOption[] options)
        {
            Internal_CreateInstance(this, ScrollBarType.ShowIfDoesntFit, ScrollBarType.ShowIfDoesntFit, scrollBarStyle, scrollAreaStyle, options);
            _mainLayout = new GUILayoutY(this);
        }

        public override void Destroy()
        {
            _mainLayout.Destroy();

            base.Destroy();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIScrollArea instance, ScrollBarType vertBarType, ScrollBarType horzBarType,
            string scrollBarStyle, string scrollAreaStyle, params GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Rect2I Internal_GetContentBounds(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetHorzScroll(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetHorzScroll(IntPtr nativeInstance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetVertScroll(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVertScroll(IntPtr nativeInstance, float value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetScrollBarWidth(IntPtr nativeInstance);
    }
}
