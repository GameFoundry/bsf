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
        internal SceneViewHandler(EditorWindow parent, Camera sceneCamera)
        {
            Internal_Create(this, parent.GetCachedPtr(), sceneCamera.Handler.GetCachedPtr());
        }

        internal void Update(Vector2I pointerPos, Vector2I inputDelta)
        {
            Internal_Update(mCachedPtr, pointerPos, inputDelta);
        }

        internal void TrySelectHandle(Vector2I pointerPos)
        {
            Internal_TrySelectHandle(mCachedPtr, pointerPos);
        }

        internal bool IsHandleActive()
        {
            return Internal_IsHandleActive(mCachedPtr);
        }

        internal void ClearHandleSelection()
        {
            Internal_ClearHandleSelection(mCachedPtr);
        }

        internal void PickObject(Vector2I pointerPos, bool controlHeld)
        {
            Internal_PickObject(mCachedPtr, pointerPos, controlHeld);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Create(SceneViewHandler managedInstance, IntPtr parentWindow, IntPtr camera);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr thisPtr, Vector2I pointerPos, Vector2I inputDelta);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_TrySelectHandle(IntPtr thisPtr, Vector2I pointerPos);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern bool Internal_IsHandleActive(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_ClearHandleSelection(IntPtr thisPtr);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_PickObject(IntPtr thisPtr, Vector2I pointerPos, bool controlHeld);
    }
}
