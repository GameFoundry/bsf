//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup GUI-Editor 
     *  @{
     */

    /// <summary>
    /// Editor GUI element that displays a textual input field and an optional label.
    /// </summary>
    public sealed class GUITextField : GUIElement
    {
        /// <summary>
        /// Triggered when the value in the field changes.
        /// </summary>
        public event Action<string> OnChanged;

        /// <summary>
        /// Triggered whenever user confirms input.
        /// </summary>
        public event Action OnConfirmed;

        /// <summary>
        /// Value displayed by the field input box.
        /// </summary>
        public string Value
        {
            get
            {
                String value;
                Internal_GetValue(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValue(mCachedPtr, value); }
        }

        /// <summary>
        /// Checks does the element currently has input focus. Input focus means the element has an input caret displayed
        /// and will accept input from the keyboard.
        /// </summary>
        public bool HasInputFocus
        {
            get
            {
                bool value;
                Internal_HasInputFocus(mCachedPtr, out value);
                return value;
            }
        }

        /// <summary>
        /// Creates a new text field element with a label.
        /// </summary>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="multiline">Determines should the input field accept multiple lines of text.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUITextField(GUIContent title, int titleWidth = 100, bool multiline = false, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, multiline, title, titleWidth, style, options, true);
        }

        /// <summary>
        /// Creates a new text field element without a label.
        /// </summary>
        /// <param name="multiline">Determines should the input field accept multiple lines of text.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUITextField(bool multiline = false, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, multiline, null, 0, style, options, false);
        }

        /// <summary>
        /// Colors the element with a specific tint.
        /// </summary>
        /// <param name="color">Tint to apply to the element.</param>
        public void SetTint(Color color)
        {
            Internal_SetTint(mCachedPtr, ref color);
        }

        /// <summary>
        /// Triggered by the runtime when the value of the text field changes.
        /// </summary>
        /// <param name="newValue">New value of the text field.</param>
        private void Internal_DoOnChanged(String newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        /// <summary>
        /// Triggered by the native interop object when the user confirms the input.
        /// </summary>
        private void Internal_DoOnConfirmed()
        {
            if (OnConfirmed != null)
                OnConfirmed();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITextField instance, bool multiline, GUIContent title, int titleWidth,
            string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out String value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, String value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_HasInputFocus(IntPtr nativeInstance, out bool value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, ref Color color);
    }

    /** @} */
}
