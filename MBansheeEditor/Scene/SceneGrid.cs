using System;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Handles rendering of the scene grid for the specified camera. Grid properties are controlled through 
    /// <see cref="EditorSettings"/>.
    /// </summary>
    internal sealed class SceneGrid : ScriptObject
    {
        /// <summary>
        /// Creates a new scene grid renderer.
        /// </summary>
        /// <param name="sceneCamera">Camera into which the grid will be rendered.</param>
        internal SceneGrid(Camera sceneCamera)
        {
            Internal_Create(this, sceneCamera.Native.GetCachedPtr());
        }

        /// <summary>
        /// Queues grid drawing for this frame.
        /// </summary>
        internal void Draw()
        {
            Internal_Draw(mCachedPtr);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(SceneGrid managedInstance, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Draw(IntPtr thisPtr);
    }
}
