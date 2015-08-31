using System;
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// A GUI element that represents a horizontal slider with a draggable handle.
    /// </summary>
    public sealed class GUISliderH : GUIElement
    {
        public delegate void OnChangedDelegate(float percent);

        /// <summary>
        /// Triggered when the slider handle moves. Provided parameter represents
        /// the position of the handle, in percent ranging [0, 1].
        /// </summary>
        public event OnChangedDelegate OnChanged;

        /// <summary>
        /// Returns the position of the slider handle, in percent ranging [0, 1].
        /// </summary>
        public float Percent
        {
            get { return Internal_GetPercent(mCachedPtr); }
            set { Internal_SetPercent(mCachedPtr, value); }
        }

        /// <summary>
        /// Creates a new horizontal slider.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUISliderH(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        /// <summary>
        /// Creates a new vertical slider.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        public GUISliderH(string style = "")
        {
            Internal_CreateInstance(this, style, new GUIOption[0]);
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
        /// Triggered by the native interop object when the slider handle is moved.
        /// </summary>
        /// <param name="percent">New position of the slider handle, in percent ranging [0, 1].</param>
        private void DoOnChanged(float percent)
        {
            if (OnChanged != null)
                OnChanged(percent);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISliderH instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPercent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPercent(IntPtr nativeInstance, float percent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }

    /// <summary>
    /// A GUI element that represents a vertical slider with a draggable handle.
    /// </summary>
    public sealed class GUISliderV : GUIElement
    {
        public delegate void OnChangedDelegate(float percent);

        /// <summary>
        /// Triggered when the slider handle moves. Provided parameter represents
        /// the position of the handle, in percent ranging [0, 1].
        /// </summary>
        public event OnChangedDelegate OnChanged;

        /// <summary>
        /// Returns the position of the slider handle, in percent ranging [0, 1].
        /// </summary>
        public float Percent
        {
            get { return Internal_GetPercent(mCachedPtr); }
            set { Internal_SetPercent(mCachedPtr, value); }
        }

        /// <summary>
        /// Creates a new vertical slider.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUISliderV(string style, params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        /// <summary>
        /// Creates a new vertical slider.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        public GUISliderV(string style = "")
        {
            Internal_CreateInstance(this, style, new GUIOption[0]);
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
        /// Triggered by the native interop object when the slider handle is moved.
        /// </summary>
        /// <param name="percent">New position of the slider handle, in percent ranging [0, 1].</param>
        private void DoOnChanged(float percent)
        {
            if (OnChanged != null)
                OnChanged(percent);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISliderV instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern float Internal_GetPercent(IntPtr nativeInstance);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetPercent(IntPtr nativeInstance, float percent);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_SetTint(IntPtr nativeInstance, Color color);
    }
}
