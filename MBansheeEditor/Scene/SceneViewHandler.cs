using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    internal sealed class SceneViewHandler : ScriptObject
    {
        internal SceneViewHandler(Camera sceneCamera)
        {
            Internal_Create(this, sceneCamera.GetCachedPtr());
        }

        internal void Update(Vector2I pointerPos)
        {
            Internal_Update(mCachedPtr, pointerPos);
        }

        internal void PointerPressed(Vector2I pointerPos)
        {
            Internal_PointerPressed(mCachedPtr, pointerPos);
        }

        internal void PointerReleased(Vector2I pointerPos, bool controlHeld)
        {
            Internal_PointerReleased(mCachedPtr, pointerPos, controlHeld);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(SceneViewHandler managedInstance, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr thisPtr, Vector2I pointerPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_PointerPressed(IntPtr thisPtr, Vector2I pointerPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_PointerReleased(IntPtr thisPtr, Vector2I pointerPos, bool controlHeld);
    }
}
