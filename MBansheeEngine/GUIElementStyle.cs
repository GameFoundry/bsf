using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;

namespace BansheeEngine
{
    public struct RectOffset
	{
		int left, right, top, bottom;
	};

	public enum GUIImagePosition
	{
		Left, Right
	};

    public enum TextHorzAlign
	{
		Left, Center, Right
	};

	public enum TextVertAlign
	{
		Top, Center, Bottom
	};

    public sealed class GUIElementStyle : ScriptObject
    {
		public GUIElementStyle()
        {
		    Internal_CreateInstance(this);
        }

        public Font font
        {
            get { return Internal_GetFont(mCachedPtr); }
            set { Internal_SetFont(mCachedPtr, value); }
        }

        public int fontSize
        {
            get { return Internal_GetFontSize(mCachedPtr); }
            set { Internal_SetFontSize(mCachedPtr, value); }
        }


        public TextHorzAlign textHorzAlign
        {
            get { return Internal_GetTextHorzAlign(mCachedPtr); }
            set { Internal_SetTextHorzAlign(mCachedPtr, value); }
        }

        public TextVertAlign textVertAlign
        {
            get { return Internal_GetTextVertAlign(mCachedPtr); }
            set { Internal_SetTextVertAlign(mCachedPtr, value); }
        }

        public GUIImagePosition imagePosition
        {
            get { return Internal_GetImagePosition(mCachedPtr); }
            set { Internal_SetImagePosition(mCachedPtr, value); }
        }

        public bool wordWrap
        {
            get { return Internal_GetWordWrap(mCachedPtr); }
            set { Internal_SetWordWrap(mCachedPtr, value); }
        }


        public GUIElementStateStyle normal
        {
            get { return Internal_GetNormal(mCachedPtr); }
            set { Internal_SetNormal(mCachedPtr, value); }
        }

        public GUIElementStateStyle hover
        {
            get { return Internal_GetHover(mCachedPtr); }
            set { Internal_SetHover(mCachedPtr, value); }
        }

        public GUIElementStateStyle active
        {
            get { return Internal_GetActive(mCachedPtr); }
            set { Internal_SetActive(mCachedPtr, value); }
        }

        public GUIElementStateStyle focused
        {
            get { return Internal_GetFocused(mCachedPtr); }
            set { Internal_SetFocused(mCachedPtr, value); }
        }


		// For controls that can be turned on-off
        public GUIElementStateStyle normalOn
        {
            get { return Internal_GetNormalOn(mCachedPtr); }
            set { Internal_SetNormalOn(mCachedPtr, value); }
        }

        public GUIElementStateStyle hoverOn
        {
            get { return Internal_GetHoverOn(mCachedPtr); }
            set { Internal_SetHoverOn(mCachedPtr, value); }
        }

        public GUIElementStateStyle activeOn
        {
            get { return Internal_GetActiveOn(mCachedPtr); }
            set { Internal_SetActiveOn(mCachedPtr, value); }
        }

        public GUIElementStateStyle focusedOn
        {
            get { return Internal_GetFocusedOn(mCachedPtr); }
            set { Internal_SetFocusedOn(mCachedPtr, value); }
        }


        public RectOffset border // Determines how the element is scaled (using the typical Scale9Grid approach)
        {
            get { return Internal_GetBorder(mCachedPtr); }
            set { Internal_SetBorder(mCachedPtr, value); }
        }

        public RectOffset margins // Determines offset from the background graphics to the content. Input uses bounds offset by this value.
        {
            get { return Internal_GetMargins(mCachedPtr); }
            set { Internal_SetMargins(mCachedPtr, value); }
        }

        public RectOffset contentOffset // Additional offset to the content, that doesn't effect the bounds. Applied on top of the margins offsets.
        {
            get { return Internal_GetContentOffset(mCachedPtr); }
            set { Internal_SetContentOffset(mCachedPtr, value); }
        }


        public int width
        {
            get { return Internal_GetWidth(mCachedPtr); }
            set { Internal_SetWidth(mCachedPtr, value); }
        }

        public int height
        {
            get { return Internal_GetHeight(mCachedPtr); }
            set { Internal_SetHeight(mCachedPtr, value); }
        }

        public int minWidth
        {
            get { return Internal_GetMinWidth(mCachedPtr); }
            set { Internal_SetMinWidth(mCachedPtr, value); }
        }
        
        public int maxWidth
        {
            get { return Internal_GetMaxWidth(mCachedPtr); }
            set { Internal_SetMaxWidth(mCachedPtr, value); }
        }

        public int minHeight
        {
            get { return Internal_GetMinHeight(mCachedPtr); }
            set { Internal_SetMinHeight(mCachedPtr, value); }
        }

        public int maxHeight
        {
            get { return Internal_GetMaxHeight(mCachedPtr); }
            set { Internal_SetMaxHeight(mCachedPtr, value); }
        }

        public bool fixedWidth
        {
            get { return Internal_GetFixedWidth(mCachedPtr); }
            set { Internal_SetFixedWidth(mCachedPtr, value); }
        }

        public bool fixedHeight
        {
            get { return Internal_GetFixedHeight(mCachedPtr); }
            set { Internal_SetFixedHeight(mCachedPtr, value); }
        }

        [DllImport("__Internal")]
        private static extern void Internal_CreateInstance(GUIElementStyle instance);

        [DllImport("__Internal")] private static extern Font Internal_GetFont(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetFont(IntPtr nativeInstance, Font value);

        [DllImport("__Internal")] private static extern int Internal_GetFontSize(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetFontSize(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern TextHorzAlign Internal_GetTextHorzAlign(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetTextHorzAlign(IntPtr nativeInstance, TextHorzAlign value);

        [DllImport("__Internal")] private static extern TextVertAlign Internal_GetTextVertAlign(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetTextVertAlign(IntPtr nativeInstance, TextVertAlign value);

        [DllImport("__Internal")] private static extern GUIImagePosition Internal_GetImagePosition(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetImagePosition(IntPtr nativeInstance, GUIImagePosition value);

        [DllImport("__Internal")] private static extern bool Internal_GetWordWrap(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetWordWrap(IntPtr nativeInstance, bool value);


        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetNormal(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetNormal(IntPtr nativeInstance, GUIElementStateStyle value);

        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetHover(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetHover(IntPtr nativeInstance, GUIElementStateStyle value);

        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetActive(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetActive(IntPtr nativeInstance, GUIElementStateStyle value);

        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetFocused(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetFocused(IntPtr nativeInstance, GUIElementStateStyle value);


        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetNormalOn(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetNormalOn(IntPtr nativeInstance, GUIElementStateStyle value);

        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetHoverOn(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetHoverOn(IntPtr nativeInstance, GUIElementStateStyle value);

        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetActiveOn(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetActiveOn(IntPtr nativeInstance, GUIElementStateStyle value);

        [DllImport("__Internal")] private static extern GUIElementStateStyle Internal_GetFocusedOn(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetFocusedOn(IntPtr nativeInstance, GUIElementStateStyle value);


        [DllImport("__Internal")] private static extern RectOffset Internal_GetBorder(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetBorder(IntPtr nativeInstance, RectOffset value);

        [DllImport("__Internal")] private static extern RectOffset Internal_GetMargins(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetMargins(IntPtr nativeInstance, RectOffset value);

        [DllImport("__Internal")] private static extern RectOffset Internal_GetContentOffset(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetContentOffset(IntPtr nativeInstance, RectOffset value);


        [DllImport("__Internal")] private static extern int Internal_GetWidth(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetWidth(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern int Internal_GetHeight(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetHeight(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern int Internal_GetMinWidth(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetMinWidth(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern int Internal_GetMaxWidth(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetMaxWidth(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern int Internal_GetMinHeight(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetMinHeight(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern int Internal_GetMaxHeight(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetMaxHeight(IntPtr nativeInstance, int value);

        [DllImport("__Internal")] private static extern bool Internal_GetFixedWidth(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetFixedWidth(IntPtr nativeInstance, bool value);

        [DllImport("__Internal")] private static extern bool Internal_GetFixedHeight(IntPtr nativeInstance);
        [DllImport("__Internal")] private static extern void Internal_SetFixedHeight(IntPtr nativeInstance, bool value);
    }
}
