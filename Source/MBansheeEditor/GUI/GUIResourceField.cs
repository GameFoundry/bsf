//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Editor GUI element that displays a reference to a <see cref="Resource"/> and an optional label. Resources can
    /// be dragged and dropped onto the field to update the reference.
    /// </summary>
    public sealed class GUIResourceField : GUIElement
    {
        public delegate void OnChangedDelegate(ResourceRef newValue);

        /// <summary>
        /// Triggered when the value in the field changes.
        /// </summary>
        public event OnChangedDelegate OnChanged;

        /// <summary>
        /// <see cref="Resource"/> referenced by the field. This will load the resource if it is not already loaded. Use
        /// <see cref="ValueRef"/> if you don't require a loaded resource.
        /// </summary>
        public Resource Value
        {
            get
            {
                Resource value;
                Internal_GetValue(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValue(mCachedPtr, value); }
        }

        /// <summary>
        /// Reference to the <see cref="Resource"/> referenced by the field.
        /// </summary>
        public ResourceRef ValueRef
        {
            get
            {
                ResourceRef value;
                Internal_GetValueRef(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValueRef(mCachedPtr, value); }
        }

        /// <summary>
        /// Creates a new resource field element with a label.
        /// </summary>
        /// <param name="type">Specific type of <see cref="Resource"/> this field accepts.</param>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIResourceField(Type type, GUIContent title, int titleWidth = 100, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, type, title, titleWidth, style, options, true);
        }

        /// <summary>
        /// Creates a new resource field element without a label.
        /// </summary>
        /// <param name="type">Specific type of <see cref="Resource"/> this field accepts.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUIResourceField(Type type, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, type, null, 0, style, options, false);
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
        /// Triggered by the runtime when the value of the field changes.
        /// </summary>
        /// <param name="newValue">New resource referenced by the field.</param>
        private void DoOnChanged(ResourceRef newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUIResourceField instance, Type type, GUIContent title, int titleWidth,
            string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out Resource value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, Resource value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValueRef(IntPtr nativeInstance, out ResourceRef value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValueRef(IntPtr nativeInstance, ResourceRef value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, ref Color color);
    }
}
