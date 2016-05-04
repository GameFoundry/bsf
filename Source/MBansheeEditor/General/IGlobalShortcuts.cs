//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
namespace BansheeEditor
{
    /** @addtogroup General 
     *  @{
     */

    /// <summary>
    /// Interface for editor windows that can respond to global shortcuts.
    /// </summary>
    internal interface IGlobalShortcuts
    {
        /// <summary>
        /// Notifies the window that the delete shortcut was pressed.
        /// </summary>
        void OnDeletePressed();

        /// <summary>
        /// Notifies the window that the rename shortcut was pressed.
        /// </summary>
        void OnRenamePressed();

        /// <summary>
        /// Notifies the window that the duplicate shortcut was pressed.
        /// </summary>
        void OnDuplicatePressed();

        /// <summary>
        /// Notifies the window that the copy shortcut was pressed.
        /// </summary>
        void OnCopyPressed();

        /// <summary>
        /// Notifies the window that the cut shortcut was pressed.
        /// </summary>
        void OnCutPressed();

        /// <summary>
        /// Notifies the window that the paste shortcut was pressed.
        /// </summary>
        void OnPastePressed();
    }

    /** @} */
}
