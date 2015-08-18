using System;
using System.Runtime.CompilerServices;

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
        // Constructor for runtime use only (dummy parameter to differentiate from the normal constructor)
        private GUIElementStyle(bool dummy)
        {  }

		public GUIElementStyle()
        {
		    Internal_CreateInstance(this);
        }

        public Font font
        {
            get { Font value; Internal_GetFont(mCachedPtr, out value); return value; }
            set { Internal_SetFont(mCachedPtr, value); }
        }

        public int fontSize
        {
            get { int value; Internal_GetFontSize(mCachedPtr, out value); return value; }
            set { Internal_SetFontSize(mCachedPtr, value); }
        }


        public TextHorzAlign textHorzAlign
        {
            get { TextHorzAlign value; Internal_GetTextHorzAlign(mCachedPtr, out value); return value; }
            set { Internal_SetTextHorzAlign(mCachedPtr, value); }
        }

        public TextVertAlign textVertAlign
        {
            get { TextVertAlign value; Internal_GetTextVertAlign(mCachedPtr, out value); return value; }
            set { Internal_SetTextVertAlign(mCachedPtr, value); }
        }

        public GUIImagePosition imagePosition
        {
            get { GUIImagePosition value; Internal_GetImagePosition(mCachedPtr, out value); return value; }
            set { Internal_SetImagePosition(mCachedPtr, value); }
        }

        public bool wordWrap
        {
            get { bool value; Internal_GetWordWrap(mCachedPtr, out value); return value; }
            set { Internal_SetWordWrap(mCachedPtr, value); }
        }


        public GUIElementStateStyle normal
        {
            get { GUIElementStateStyle value; Internal_GetNormal(mCachedPtr, out value); return value; }
            set { Internal_SetNormal(mCachedPtr, value); }
        }

        public GUIElementStateStyle hover
        {
            get { GUIElementStateStyle value; Internal_GetHover(mCachedPtr, out value); return value; }
            set { Internal_SetHover(mCachedPtr, value); }
        }

        public GUIElementStateStyle active
        {
            get { GUIElementStateStyle value; Internal_GetActive(mCachedPtr, out value); return value; }
            set { Internal_SetActive(mCachedPtr, value); }
        }

        public GUIElementStateStyle focused
        {
            get { GUIElementStateStyle value; Internal_GetFocused(mCachedPtr, out value); return value; }
            set { Internal_SetFocused(mCachedPtr, value); }
        }


		// For controls that can be turned on-off
        public GUIElementStateStyle normalOn
        {
            get { GUIElementStateStyle value; Internal_GetNormalOn(mCachedPtr, out value); return value; }
            set { Internal_SetNormalOn(mCachedPtr, value); }
        }

        public GUIElementStateStyle hoverOn
        {
            get { GUIElementStateStyle value; Internal_GetHoverOn(mCachedPtr, out value); return value; }
            set { Internal_SetHoverOn(mCachedPtr, value); }
        }

        public GUIElementStateStyle activeOn
        {
            get { GUIElementStateStyle value; Internal_GetActiveOn(mCachedPtr, out value); return value; }
            set { Internal_SetActiveOn(mCachedPtr, value); }
        }

        public GUIElementStateStyle focusedOn
        {
            get { GUIElementStateStyle value; Internal_GetFocusedOn(mCachedPtr, out value); return value; }
            set { Internal_SetFocusedOn(mCachedPtr, value); }
        }


        public RectOffset border // Determines how the element is scaled (using the typical Scale9Grid approach)
        {
            get { RectOffset value; Internal_GetBorder(mCachedPtr, out value); return value; }
            set { Internal_SetBorder(mCachedPtr, ref value); }
        }

        public RectOffset margins // Determines offset from the background graphics to the content. Input uses bounds offset by this value.
        {
            get { RectOffset value; Internal_GetMargins(mCachedPtr, out value); return value; }
            set { Internal_SetMargins(mCachedPtr, ref value); }
        }

        public RectOffset contentOffset // Additional offset to the content, that doesn't effect the bounds. Applied on top of the margins offsets.
        {
            get { RectOffset value; Internal_GetContentOffset(mCachedPtr, out value); return value; }
            set { Internal_SetContentOffset(mCachedPtr, ref value); }
        }


        public int width
        {
            get { int value; Internal_GetWidth(mCachedPtr, out value); return value; }
            set { Internal_SetWidth(mCachedPtr, value); }
        }

        public int height
        {
            get { int value; Internal_GetHeight(mCachedPtr, out value); return value; }
            set { Internal_SetHeight(mCachedPtr, value); }
        }

        public int minWidth
        {
            get { int value; Internal_GetMinWidth(mCachedPtr, out value); return value; }
            set { Internal_SetMinWidth(mCachedPtr, value); }
        }
        
        public int maxWidth
        {
            get { int value; Internal_GetMaxWidth(mCachedPtr, out value); return value; }
            set { Internal_SetMaxWidth(mCachedPtr, value); }
        }

        public int minHeight
        {
            get { int value; Internal_GetMinHeight(mCachedPtr, out value); return value; }
            set { Internal_SetMinHeight(mCachedPtr, value); }
        }

        public int maxHeight
        {
            get { int value; Internal_GetMaxHeight(mCachedPtr, out value); return value; }
            set { Internal_SetMaxHeight(mCachedPtr, value); }
        }

        public bool fixedWidth
        {
            get { bool value; Internal_GetFixedWidth(mCachedPtr, out value); return value; }
            set { Internal_SetFixedWidth(mCachedPtr, value); }
        }

        public bool fixedHeight
        {
            get { bool value; Internal_GetFixedHeight(mCachedPtr, out value); return value; }
            set { Internal_SetFixedHeight(mCachedPtr, value); }
        }

        public void AddSubStyle(string guiType, string styleName)
        {
            if (guiType == null || styleName == null)
                return;

            Internal_AddSubStyle(mCachedPtr, guiType, styleName);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIElementStyle instance);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetFont(IntPtr nativeInstance, out Font value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetFont(IntPtr nativeInstance, Font value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetFontSize(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetFontSize(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetTextHorzAlign(IntPtr nativeInstance, out TextHorzAlign value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetTextHorzAlign(IntPtr nativeInstance, TextHorzAlign value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetTextVertAlign(IntPtr nativeInstance, out TextVertAlign value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetTextVertAlign(IntPtr nativeInstance, TextVertAlign value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetImagePosition(IntPtr nativeInstance, out GUIImagePosition value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetImagePosition(IntPtr nativeInstance, GUIImagePosition value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetWordWrap(IntPtr nativeInstance, out bool value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetWordWrap(IntPtr nativeInstance, bool value);


        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetNormal(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetNormal(IntPtr nativeInstance, GUIElementStateStyle value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetHover(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetHover(IntPtr nativeInstance, GUIElementStateStyle value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetActive(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetActive(IntPtr nativeInstance, GUIElementStateStyle value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetFocused(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetFocused(IntPtr nativeInstance, GUIElementStateStyle value);


        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetNormalOn(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetNormalOn(IntPtr nativeInstance, GUIElementStateStyle value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetHoverOn(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetHoverOn(IntPtr nativeInstance, GUIElementStateStyle value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetActiveOn(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetActiveOn(IntPtr nativeInstance, GUIElementStateStyle value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetFocusedOn(IntPtr nativeInstance, out GUIElementStateStyle value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetFocusedOn(IntPtr nativeInstance, GUIElementStateStyle value);


        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetBorder(IntPtr nativeInstance, out RectOffset value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetBorder(IntPtr nativeInstance, ref RectOffset value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetMargins(IntPtr nativeInstance, out RectOffset value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetMargins(IntPtr nativeInstance, ref RectOffset value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetContentOffset(IntPtr nativeInstance, out RectOffset value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetContentOffset(IntPtr nativeInstance, ref RectOffset value);


        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetWidth(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetWidth(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetHeight(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetHeight(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetMinWidth(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetMinWidth(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetMaxWidth(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetMaxWidth(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetMinHeight(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetMinHeight(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetMaxHeight(IntPtr nativeInstance, out int value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetMaxHeight(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetFixedWidth(IntPtr nativeInstance, out bool value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetFixedWidth(IntPtr nativeInstance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_GetFixedHeight(IntPtr nativeInstance, out bool value);
        [MethodImpl(MethodImplOptions.InternalCall)] 
        private static extern void Internal_SetFixedHeight(IntPtr nativeInstance, bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_AddSubStyle(IntPtr nativeInstance, string guiType, string styleName);
    }
}
