using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// GUI element that displays all scene objects in the current scene as a tree view.
    /// </summary>
    public sealed class GUISceneTreeView : GUIElement
    {
        /// <summary>
        /// Creates a new scene tree view element.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUISceneTreeView(string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        /// <summary>
        /// Creates a new scene tree view element.
        /// </summary>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUISceneTreeView(params GUIOption[] options)
        {
            Internal_CreateInstance(this, "", options);
        }

        /// <summary>
        /// Updates the contents of the tree view with most recent scene data. Should be called once per frame.
        /// </summary>
        public void Update()
        {
            Internal_Update(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISceneTreeView instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr thisPtr);
    }
}
