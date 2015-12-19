using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles scene view handle interactions, object picking and gizmos.
    /// </summary>
    internal sealed class SceneViewHandler : ScriptObject
    {
        /// <summary>
        /// Creates a new scene view handler.
        /// </summary>
        /// <param name="parent">Editor window in which the scene view is displayed.</param>
        /// <param name="sceneCamera">Camera through which the scene view is displayed.</param>
        internal SceneViewHandler(EditorWindow parent, Camera sceneCamera)
        {
            Internal_Create(this, parent.GetCachedPtr(), sceneCamera.Native.GetCachedPtr());
        }

        /// <summary>
        /// Called every frame. Updates gizmos and scene grid.
        /// </summary>
        internal void Update()
        {
            Internal_Update(mCachedPtr);
        }

        /// <summary>
        /// Updates currently active handles.
        /// </summary>
        /// <param name="pointerPos">Position of the pointer relative to the scene camera viewport.</param>
        /// <param name="inputDelta">Movement of the pointer since last frame.</param>
        internal void UpdateHandle(Vector2I pointerPos, Vector2I inputDelta)
        {
            Internal_UpdateHandle(mCachedPtr, ref pointerPos, ref inputDelta);
        }

        /// <summary>
        /// Updates the selection overlay for currently selected object(s).
        /// </summary>
        internal void UpdateSelection()
        {
            Internal_UpdateSelection(mCachedPtr);
        }

        /// <summary>
        /// Selects a handle under the pointer position.
        /// </summary>
        /// <param name="pointerPos">Position of the pointer relative to the scene camera viewport.</param>
        internal void TrySelectHandle(Vector2I pointerPos)
        {
            Internal_TrySelectHandle(mCachedPtr, ref pointerPos);
        }

        /// <summary>
        /// Checks is any handle currently active.
        /// </summary>
        /// <returns>True if a handle is active.</returns>
        internal bool IsHandleActive()
        {
            return Internal_IsHandleActive(mCachedPtr);
        }

        /// <summary>
        /// Deselects any currently active handles.
        /// </summary>
        internal void ClearHandleSelection()
        {
            Internal_ClearHandleSelection(mCachedPtr);
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
        private static extern void Internal_Create(SceneViewHandler managedInstance, IntPtr parentWindow, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UpdateHandle(IntPtr thisPtr, ref Vector2I pointerPos, ref Vector2I inputDelta);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_UpdateSelection(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_TrySelectHandle(IntPtr thisPtr, ref Vector2I pointerPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsHandleActive(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearHandleSelection(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_PickObject(IntPtr thisPtr, ref Vector2I pointerPos, bool controlHeld);
    }
}
