//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System.Runtime.InteropServices;

namespace BansheeEngine
{
    /** @addtogroup GUI_Engine
     *  @{
     */

    /// <summary>
    /// A key combination that is used for triggering keyboard shortcuts. Contains a button code and an optional modifier.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ShortcutKey // Note: Must match C++ class ShortcutKey
    {
        /// <summary>
        /// Optional modifier that is required to be pressed along with the shortcut button
        /// </summary>
        public ButtonModifier modifier;

        /// <summary>
        /// Shortcut button that triggers the shortcut.
        /// </summary>
        public ButtonCode key;

        /// <summary>
        /// Creates a new shortcut key.
        /// </summary>
        /// <param name="modifier">Optional modifier that is required to be pressed along with the shortcut button.</param>
        /// <param name="key">Shortcut button that triggers the shortcut.</param>
        public ShortcutKey(ButtonModifier modifier, ButtonCode key)
        {
            this.modifier = modifier;
            this.key = key;
        }

        /// <summary>
        /// Blank shortcut key that is triggered by no key combination.
        /// </summary>
        public static ShortcutKey None = new ShortcutKey(ButtonModifier.None, ButtonCode.Unassigned);
    }

    /** @} */
}
