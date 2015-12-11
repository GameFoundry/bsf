using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Editor GUI element that displays a reference to a <see cref="Texture"/> and an optional label. Textures can
    /// be dragged and dropped onto the field to update the reference. This is similar to <see cref="GUIResourceField"/>
    /// but the will display the texture contents and not only the name.
    /// </summary>
    public sealed class GUITextureField : GUIElement
    {
        public delegate void OnChangedDelegate(ResourceRef newValue);

        /// <summary>
        /// Triggered when the value in the field changes.
        /// </summary>
        public event OnChangedDelegate OnChanged;

        /// <summary>
        /// <see cref="Texture"/> referenced by the field. This will load the texture if it is not already loaded. Use
        /// <see cref="ValueRef"/> if you don't require a loaded resource.
        /// </summary>
        public Texture Value
        {
            get
            {
                Texture value;
                Internal_GetValue(mCachedPtr, out value);
                return value;
            }

            set { Internal_SetValue(mCachedPtr, value); }
        }

        /// <summary>
        /// Reference to the <see cref="Texture"/> referenced by the field.
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
        /// Creates a new texture field element with a label.
        /// </summary>
        /// <param name="title">Content to display on the label.</param>
        /// <param name="titleWidth">Width of the title label in pixels.</param>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUITextureField(GUIContent title, int titleWidth = 100, string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, title, titleWidth, style, options, true);
        }

        /// <summary>
        /// Creates a new texture field element without a label.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUITextureField(string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, null, 0, style, options, false);
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
        /// Triggered by the runtime when the value of the field changes.
        /// </summary>
        /// <param name="newValue">New resource referenced by the field.</param>
        private void Internal_DoOnChanged(ResourceRef newValue)
        {
            if (OnChanged != null)
                OnChanged(newValue);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUITextureField instance, GUIContent title, int titleWidth,
            string style, GUIOption[] options, bool withTitle);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValue(IntPtr nativeInstance, out Texture value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValue(IntPtr nativeInstance, Texture value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_GetValueRef(IntPtr nativeInstance, out ResourceRef value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetValueRef(IntPtr nativeInstance, ResourceRef value);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
