using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Editor GUI element that displays a list box with user-specified elements and an optional label.
    /// </summary>
    public sealed class GUIListBoxField : GUIElement
    {
        public delegate void OnSelectionChangedDelegate(int index);

        /// <summary>
        /// Triggered whenever user selects a new element in the list box. Returned index maps to the element in the
        /// elements array that the list box was initialized with.
        /// </summary>
        public event OnSelectionChangedDelegate OnSelectionChanged;

        /// <summary>
        /// Index of the list box entry currently selected.
        /// </summary>
        public int Index
        {
            get { return Internal_GetValue(mCachedPtr); }
            set { Internal_SetValue(mCachedPtr, value); }
        }

        /// <summary>
        /// Creates a new list box with the specified elements and a label.
        /// </summary>
        /// <param name="elements">Array of elements to display in the list box. Elements will be displayed in the same 
        ///                        order as in the array.</param>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIListBoxField(LocString[] elements, GUIContent title, int titleWidth = 100, string style = "", 
            params GUIOption[] options)
        {
            Internal_CreateInstance(this, elements, title, titleWidth, style, options, true);
        }

        /// <summary>
        /// Creates a new list box with the specified elements and a label.
        /// </summary>
        /// <param name="elements">Array of elements to display in the list box. Elements will be displayed in the same 
        ///                        order as in the array.</param>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIListBoxField(LocString[] elements, GUIContent title, int titleWidth = 100, params GUIOption[] options)
        {
            Internal_CreateInstance(this, elements, title, titleWidth, "", options, true);
        }

        /// <summary>
        /// Creates a new list box with the specified elements and no label.
        /// </summary>
        /// <param name="elements">Array of elements to display in the list box. Elements will be displayed in the same 
        ///                        order as in the array.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIListBoxField(LocString[] elements, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, elements, null, 0, style, options, false);
        }

        /// <summary>
        /// Creates a new list box with the specified elements and no label.
        /// </summary>
        /// <param name="elements">Array of elements to display in the list box. Elements will be displayed in the same 
        ///                        order as in the array.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIListBoxField(LocString[] elements, params GUIOption[] options)
        {
            Internal_CreateInstance(this, elements, null, 0, "", options, false);
        }

        /// <summary>
        /// Updates the list box with a new set of elements.
        /// </summary>
        /// <param name="elements">Array of elements to display in the list box. Elements will be displayed in the same 
        ///                        order as in the array.</param>
        public void SetElements(LocString[] elements)
        {
            Internal_SetElements(mCachedPtr, elements);
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
        private static extern void Internal_CreateInstance(GUIListBoxField instance, LocString[] entries,
            GUIContent title, int titleWidth, string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetElements(IntPtr nativeInstance, LocString[] elements);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern int Internal_GetValue(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, int value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
