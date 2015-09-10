using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// GUI element that displays a foldout button that can be expanded or collapsed.
    /// </summary>
    public sealed class GUIComponentFoldout : GUIElement
    {
        /// <summary>
        /// Triggered when the foldout is expanded or collapsed.
        /// </summary>
        public event Action<bool> OnToggled;

        /// <summary>
        /// Creates a new foldout element.
        /// </summary>
        /// <param name="content">Content to display on the button.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIComponentFoldout(GUIContent content, string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, style, options);
        }

        /// <summary>
        /// Creates a new foldout element.
        /// </summary>
        /// <param name="content">Content to display on the button.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        public GUIComponentFoldout(GUIContent content, string style)
        {
            Internal_CreateInstance(this, content, style, new GUIOption[0]);
        }

        /// <summary>
        /// Creates a new foldout element.
        /// </summary>
        /// <param name="content">Content to display on the button.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIComponentFoldout(GUIContent content, params GUIOption[] options)
        {
            Internal_CreateInstance(this, content, "", options);
        }

        /// <summary>
        /// Changes the contents displayed on the button.
        /// </summary>
        /// <param name="content">Content to display on the button.</param>
        public void SetContent(GUIContent content)
        {
            Internal_SetContent(mCachedPtr, content);
        }

        /// <summary>
        /// Determines is the foldout expanded or collapsed.
        /// </summary>
        public bool Expanded
        {
            get
            {
                bool expanded;
                Internal_IsExpanded(mCachedPtr, out expanded);
                return expanded;
            }
            set
            {
                Internal_SetExpanded(mCachedPtr, value);
            }
        }

        /// <summary>
        /// Colors the element with a specific tint.
        /// </summary>
        /// <param name="color">Tint to apply to the element.</param>
        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, color);
        }

        /// <summary>
        /// Triggered by the runtime when the foldout button is toggled.
        /// </summary>
        /// <param name="expanded">True if the foldout has been expanded, false if collapsed.</param>
        private void DoOnToggled(bool expanded)
        {
            if (OnToggled != null)
                OnToggled(expanded);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIComponentFoldout instance, GUIContent content, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetContent(IntPtr nativeInstance, GUIContent content);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetExpanded(IntPtr nativeInstance, bool expanded);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_IsExpanded(IntPtr nativeInstance, out bool expanded);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
