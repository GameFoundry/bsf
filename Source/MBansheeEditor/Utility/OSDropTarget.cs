//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Utility-Editor
     *  @{
     */

    /// <summary>
    /// Drop targets allow you to register a certain portion of a window as a drop target that accepts file drops from the
    /// OS (platform) specific drag and drop system. You will receive events with the specified drop area as long as it is 
    /// active.
    /// </summary>
    public class OSDropTarget : ScriptObject
    {
        /// <summary>
        /// Triggered when the user completes a drop while pointer is over the drop area. Provides window coordinates of 
        /// the pointer position.
        /// </summary>
        public Action<int, int> OnDrop;

        /// <summary>
        /// Triggered when a pointer enters the drop area. Provides window coordinates of the pointer position.
        /// </summary>
        public Action<int, int> OnEnter;

        /// <summary>
        /// Triggered when a pointer leaves the drop area.
        /// </summary>
        public Action OnLeave;

        /// <summary>
        /// Triggered when a pointer is being dragged over the drop area. Provides window coordinates of the pointer 
        /// position.
        /// </summary>
        public Action<int, int> OnDrag;

        /// <summary>
        /// Creates a new OS drop target.
        /// </summary>
        /// <param name="window">Window on which the drop target is on.</param>
        public OSDropTarget(EditorWindow window)
        {
            IntPtr nativeWindow = window.GetCachedPtr();;
            Internal_CreateInstance(this, nativeWindow);
        }

        /// <summary>
        /// Destroys the drop target, stopping any further events.
        /// </summary>
        public void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        /// <summary>
        /// Allows you to set the drop target bounds, relative to the parent window.
        /// </summary>
        public Rect2I Bounds
        {
            set { Internal_SetBounds(mCachedPtr, ref value); }
        }

        /// <summary>
        /// Returns a list of files received by the drop target. Only valid after a drop has been triggered.
        /// </summary>
        public string[] FilePaths
        {
            get { return Internal_GetFilePaths(mCachedPtr); }
        }

        /// <summary>
        /// Triggered internally by the runtime when a pointer enters the drop area.
        /// </summary>
        /// <param name="x">X coordinate of the pointer position, relative to the parent window.</param>
        /// <param name="y">Y coordinate of the pointer position, relative to the parent window.</param>
        private void InternalDoOnEnter(int x, int y)
        {
            if (OnEnter != null)
                OnEnter(x, y);
        }

        /// <summary>
        /// Triggered internally by the runtime when a pointer leaves the drop area.
        /// </summary>
        private void InternalDoOnLeave()
        {
            if (OnLeave != null)
                OnLeave();
        }

        /// <summary>
        /// Triggered internally by the runtime when a pointer is being dragged over the drop area.
        /// </summary>
        /// <param name="x">X coordinate of the pointer position, relative to the parent window.</param>
        /// <param name="y">Y coordinate of the pointer position, relative to the parent window.</param>
        private void InternalDoOnDrag(int x, int y)
        {
            if (OnDrag != null)
                OnDrag(x, y);
        }

        /// <summary>
        /// Triggered internally by the runtime when the user completes a drop while pointer is over the drop area.
        /// </summary>
        /// <param name="x">X coordinate of the pointer position, relative to the parent window.</param>
        /// <param name="y">Y coordinate of the pointer position, relative to the parent window.</param>
        private void InternalDoOnDrop(int x, int y)
        {
            if (OnDrop != null)
                OnDrop(x, y);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(OSDropTarget instance, IntPtr editorWindow);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetBounds(IntPtr nativeInstance, ref Rect2I bounds);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string[] Internal_GetFilePaths(IntPtr nativeInstance);
    }

    /** @} */
}
