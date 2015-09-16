using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Editor GUI element that displays a list box with elements of an enumeration and an optional label.
    /// </summary>
    public sealed class GUIEnumField : GUIElement
    {
        public delegate void OnSelectionChangedDelegate(int value);

        /// <summary>
        /// Triggered whenever user selects a new element in the list box. Returns the value of the enumeration entry that 
        /// was selected.
        /// </summary>
        public event OnSelectionChangedDelegate OnSelectionChanged;

        /// <summary>
        /// Value of the enumeration entry currently selected.
        /// </summary>
        public int Value
        {
            get { return Internal_GetValue(mCachedPtr); }
            set { Internal_SetValue(mCachedPtr, value); }
        }

        /// <summary>
        /// Creates a new list box with enumeration entries as its elements and a label.
        /// </summary>
        /// <param name="enumType">Type of enum of whose entries to display in the list box.</param>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIEnumField(Type enumType, GUIContent title, int titleWidth = 100, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, Enum.GetNames(enumType), Enum.GetValues(enumType), title, titleWidth, style, options, true);
        }

        /// <summary>
        /// Creates a new list box with enumeration entries as its elements and a label.
        /// </summary>
        /// <param name="enumType">Type of enum of whose entries to display in the list box.</param>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIEnumField(Type enumType, GUIContent title, int titleWidth = 100, params GUIOption[] options)
        {
            Internal_CreateInstance(this, Enum.GetNames(enumType), Enum.GetValues(enumType), title, titleWidth, "", options, true);
        }

        /// <summary>
        /// Creates a new list box with enumeration entries as its elements and no label.
        /// </summary>
        /// <param name="enumType">Type of enum of whose entries to display in the list box.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIEnumField(Type enumType, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, Enum.GetNames(enumType), Enum.GetValues(enumType), null, 0, style, options, false);
        }

        /// <summary>
        /// Creates a new list box with enumeration entries as its elements and no label.
        /// </summary>
        /// <param name="enumType">Type of enum of whose entries to display in the list box.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIEnumField(Type enumType, params GUIOption[] options)
        {
            Internal_CreateInstance(this, Enum.GetNames(enumType), Enum.GetValues(enumType), null, 0, "", options, false);
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
        /// Triggered by the native interop object when a user selects an object in the list.
        /// </summary>
        private void DoOnSelectionChanged(int index)
        {
            if (OnSelectionChanged != null)
                OnSelectionChanged(index);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIEnumField instance, string[] names, Array values, 
            GUIContent title, int titleWidth, string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetValue(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
