using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Wrapper around the native GUIWidget class.
    /// <see cref="GUIWidget"/>
    /// </summary>
    internal class NativeGUIWidget : ScriptObject
    {
        /// <summary>
        /// Skin used for rendering all the GUI elements belonging to this widget.
        /// </summary>
        internal GUISkin Skin
        {
            get { return Internal_GetSkin(mCachedPtr); }
            set
            {
                IntPtr skinPtr = IntPtr.Zero;
                if (value != null)
                    skinPtr = value.GetCachedPtr();

                Internal_SetSkin(mCachedPtr, skinPtr);
            }
        }

        /// <summary>
        /// Determines to which camera are the GUI elements belonong to this widget rendered. If null then they will be
        /// rendered on the main camera.
        /// </summary>
        internal Camera Camera
        {
            get { return Internal_GetCamera(mCachedPtr); }
            set
            {
                IntPtr cameraPtr = IntPtr.Zero;
                if (value != null)
                    cameraPtr = value.GetCachedPtr();

                Internal_SetCamera(mCachedPtr, cameraPtr);
            }
        }

        /// <summary>
        /// Container into which all GUI elements belonging to this widget should be placed.
        /// </summary>
        internal GUIPanel Panel
        {
            get { return Internal_GetPanel(mCachedPtr); }
        }

        /// <summary>
        /// Deletes the GUI widget and all child GUI elements.
        /// </summary>
        internal void Destroy()
        {
            Internal_Destroy(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GUISkin Internal_GetSkin(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSkin(IntPtr instance, IntPtr skin);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern Camera Internal_GetCamera(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetCamera(IntPtr instance, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GUIPanel Internal_GetPanel(IntPtr instance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Destroy(IntPtr instance);
    }
}
