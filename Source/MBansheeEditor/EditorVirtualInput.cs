//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.CompilerServices;

namespace BansheeEngine
{
    /// <summary>
    /// Handles virtual input that allows you to receive virtual input events that hide the actual physical input, allowing 
    /// you to easily change the input keys while being transparent to the external code.
    /// </summary>
    /// <summary>
    /// Companion class to <see cref="VirtualInput"/> for use in editor only. Supplies events that trigger regardless 
    /// whether game is playing or not (unlike <see cref="VirtualInput"/>) which makes them usable for editor only scripts. 
    /// Pollable input and other functionality should still be accessed on <see cref="VirtualInput"/>.
    /// </summary>
    public static class EditorVirtualInput
    {
        public delegate void OnButtonEventDelegate(VirtualButton btn, int deviceIdx);

        /// <summary>
        /// Triggered when a physical button combination corresponding to a virtual button is pressed.
        /// </summary>
        public static event OnButtonEventDelegate OnButtonDown;

        /// <summary>
        /// Triggered when a physical button combination corresponding to a virtual button is released.
        /// </summary>
        public static event OnButtonEventDelegate OnButtonUp;

        /// <summary>
        /// Triggered every frame while a physical button combination corresponding to a virtual button is being held down.
        /// </summary>
        public static event OnButtonEventDelegate OnButtonHeld;

        /// <summary>
        /// Triggered by the runtime when the virtual button is pressed.
        /// </summary>
        /// <param name="button">Virtual button that was pressed.</param>
        /// <param name="deviceIdx">Index of the device the button was pressed on.</param>
        private static void Internal_TriggerButtonDown(VirtualButton button, int deviceIdx)
        {
            if (OnButtonDown != null)
                OnButtonDown(button, deviceIdx);
        }

        /// <summary>
        /// Triggered by the runtime when the virtual button is released.
        /// </summary>
        /// <param name="button">Virtual button that was released.</param>
        /// <param name="deviceIdx">Index of the device the button was released on.</param>
        private static void Internal_TriggerButtonUp(VirtualButton button, int deviceIdx)
        {
            if (OnButtonUp != null)
                OnButtonUp(button, deviceIdx);
        }

        /// <summary>
        /// Triggered by the runtime every frame while a virtual button is being held down.
        /// </summary>
        /// <param name="button">Virtual button that is being held down.</param>
        /// <param name="deviceIdx">Index of the device the button is being held down on.</param>
        private static void Internal_TriggerButtonHeld(VirtualButton button, int deviceIdx)
        {
            if (OnButtonHeld != null)
                OnButtonHeld(button, deviceIdx);
        }
    };
}
