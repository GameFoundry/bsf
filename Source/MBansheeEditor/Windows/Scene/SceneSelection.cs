//********************************** Banshee Engine (www.banshee3d.com) **************************************************//
//**************** Copyright (c) 2016 Marko Pintera (marko.pintera@gmail.com). All rights reserved. **********************//
using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /** @addtogroup Scene-Editor
     *  @{
     */

    /// <summary>
    /// Handles rendering of the selection overlay and picking of objects in the target camera's view.
    /// </summary>
    internal sealed class SceneSelection : ScriptObject
    {
        /// <summary>
        /// Creates a new scene selection manager.
        /// </summary>
        /// <param name="sceneCamera">Camera into which to render the selection overlay, and perform picking from.</param>
        internal SceneSelection(Camera sceneCamera)
        {
            Internal_Create(this, sceneCamera.Native.GetCachedPtr());
        }

        /// <summary>
        /// Queues selection overlay drawing for this frame.
        /// </summary>
        internal void Draw()
        {
            Internal_Draw(mCachedPtr);
        }

        /// <summary>
        /// Attempts to select a scene object under the pointer position.
        /// </summary>
        /// <param name="pointerPos">Position of the pointer relative to the scene camera viewport.</param>
        /// <param name="controlHeld">Should this selection add to the existing selection, or replace it.</param>
        internal void PickObject(Vector2I pointerPos, bool controlHeld)
        {
            Internal_PickObject(mCachedPtr, ref pointerPos, controlHeld);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(SceneSelection managedInstance, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Draw(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_PickObject(IntPtr thisPtr, ref Vector2I pointerPos, bool controlHeld);
    }

    /** @} */
}
