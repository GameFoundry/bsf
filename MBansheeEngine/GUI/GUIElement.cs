using System;
using System.Collections.Generic;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Base class for all GUI elements. Every GUI element can at least be positioned in it's parent layout/panel and be 
    /// hidden/visible, focused/unfocused and assigned a context menu.
    /// </summary>
    public abstract class GUIElement : ScriptObject
    {
        /// <summary>
        /// Triggered when a GUI element receives or loses keyboard focus.
        /// </summary>
        public Action<bool> OnFocusChanged;

        /// <summary>
        /// Gets or sets non-clipped bounds of the GUI element. Relative to a parent GUI panel.
        /// </summary>
        public Rect2I Bounds
        {
            get { return Internal_GetBounds(mCachedPtr); }
            set { Internal_SetBounds(mCachedPtr, value); }
        }

        /// <summary>
        /// Gets or sets non-clipped bounds of the GUI element including the margins. Relative to a parent GUI panel.
        /// </summary>
        public Rect2I VisualBounds
        {
            get { return Internal_GetVisualBounds(mCachedPtr); }
        }

        /// <summary>
        /// Makes the element hidden or visible.
        /// </summary>
        public bool Visible
        {
            set { Internal_SetVisible(mCachedPtr, value); }
        }

        /// <summary>
        /// Assigns or removes keyboard focus on this element.
        /// </summary>
        public bool Focus
        {
            set { Internal_SetFocus(mCachedPtr, value); }
        }

        /// <summary>
        /// Destroys this element and all its children. Removes the element from parent layout/panel.
        /// </summary>
        /// <remarks>
        /// Calling methods on a destroyed element is a no-operation. Destroyed elements aren't allowed to be assigned as 
        /// children of other elements, or be parents of other elements.
        /// </remarks>
        public virtual void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        /// <summary>
        /// Sets element position relative to parent GUI panel.
        /// </summary>
        /// <param name="x">X position of the element in pixels, relative to parent GUI panel.</param>
        /// <param name="y">Y position of the element in pixels, relative to parent GUI panel.</param>
        /// <remarks>
        /// Be aware that this value will be ignored if GUI element is part of a layout because the layout controls placement 
        /// of child elements.
        /// </remarks>
        public void SetPosition(int x, int y)
        {
            Internal_SetPosition(mCachedPtr, x, y);
        }

        /// <summary>
        /// Sets a fixed element width.
        /// </summary>
        /// <param name="width">Width in pixels.</param>
        public void SetWidth(int width)
        {
            Internal_SetWidth(mCachedPtr, width);
        }

        /// <summary>
        /// Sets a flexible element width. Element will be resized according to its contents and parent layout but will 
        /// always stay within the provided range.
        /// </summary>
        /// <param name="minWidth">Minimum width in pixels. Element will never be smaller than this width.</param>
        /// <param name="maxWidth">Maximum width in pixels. Element will never be larger than this width. Specify zero for 
        ///                        unlimited width.</param>
        public void SetFlexibleWidth(int minWidth, int maxWidth)
        {
            Internal_SetFlexibleWidth(mCachedPtr, minWidth, maxWidth);
        }

        /// <summary>
        /// Sets a fixed element height.
        /// </summary>
        /// <param name="height">Height in pixels.</param>
        public void SetHeight(int height)
        {
            Internal_SetHeight(mCachedPtr, height);
        }

        /// <summary>
        /// Sets a flexible element height. Element will be resized according to its contents and parent layout but will 
        /// always stay within the provided range.
        /// </summary>
        /// <param name="minHeight">Minimum height in pixels. Element will never be smaller than this height.</param>
        /// <param name="maxHeight">Maximum height in pixels. Element will never be larger than this height. Specify zero 
        ///                         for unlimited height.</param>
        public void SetFlexibleHeight(int minHeight, int maxHeight)
        {
            Internal_SetFlexibleHeight(mCachedPtr, minHeight, maxHeight);
        }

        /// <summary>
        /// Resets element bounds to their initial values dictated by the element's style.
        /// </summary>
        public void ResetDimensions()
        {
            Internal_ResetDimensions(mCachedPtr);
        }

        /// <summary>
        /// Assigns a new context menu that will be opened when the element is right clicked.
        /// </summary>
        /// <param name="menu">Object containing context menu contents. Can be null if no menu is wanted.</param>
        public void SetContextMenu(ContextMenu menu)
        {
            IntPtr menuPtr = IntPtr.Zero;
            if (menu != null)
                menuPtr = menu.GetCachedPtr();

            Internal_SetContextMenu(mCachedPtr, menuPtr);
        }

        /// <summary>
        /// Triggered by the native interop object when the keyboard focus of this element changes.
        /// </summary>
        private void InternalOnFocusChanged(bool focus)
        {
            if (OnFocusChanged != null)
                OnFocusChanged(focus);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetVisible(IntPtr nativeInstance, bool visible);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFocus(IntPtr nativeInstance, bool focus);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPosition(IntPtr nativeInstance, int x, int y);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetWidth(IntPtr nativeInstance, int width);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFlexibleWidth(IntPtr nativeInstance, int minWidth, int maxWidth);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetHeight(IntPtr nativeInstance, int height);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetFlexibleHeight(IntPtr nativeInstance, int minHeight, int maxHeight);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ResetDimensions(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Rect2I Internal_GetBounds(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBounds(IntPtr nativeInstance, Rect2I value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Rect2I Internal_GetVisualBounds(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContextMenu(IntPtr nativeInstance, IntPtr contextMenu);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);
    }
}
