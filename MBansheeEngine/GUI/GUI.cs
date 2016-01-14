using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Provides access to a global <see cref="GUIPanel"/> that renders GUI on the main viewport. Use 
    /// <see cref="GUIWidget"/> if you need more control over the placement of GUI, or require it to be rendered to a 
    /// different viewport.
    /// </summary>
    public static class GUI
    {
        /// <summary>
        /// Skin used for rendering all the GUI elements.
        /// </summary>
        public static GUISkin Skin
        {
            get { return Internal_GetSkin(); }
            set
            {
                IntPtr skinPtr = IntPtr.Zero;
                if (value != null)
                    skinPtr = value.GetCachedPtr();

                Internal_SetSkin(skinPtr);
            }
        }

        /// <summary>
        /// Container into which all GUI elements should be placed.
        /// </summary>
        public static GUIPanel Panel
        {
            get { return Internal_GetPanel(); }
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GUISkin Internal_GetSkin();

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetSkin(IntPtr skin);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern GUIPanel Internal_GetPanel();
    }
}
