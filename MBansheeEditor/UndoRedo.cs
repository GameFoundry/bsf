using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    public static class UndoRedo
    {
        public static void Undo()
        {
            Internal_Undo();
        }

        public static void Redo()
        {
            Internal_Redo();
        }

        public static void RecordSO(SceneObject so, string description = "")
        {
            if (so != null)
                Internal_RecordSO(so.GetCachedPtr(), description);
        }

        public static SceneObject[] CloneSO(SceneObject[] so, string description = "")
        {
            if (so != null)
            {
                List<IntPtr> soPtrs = new List<IntPtr>();
                for (int i = 0; i < so.Length; i++)
                {
                    if(so[i] != null)
                        soPtrs.Add(so[i].GetCachedPtr());
                }

                return Internal_CloneSOMulti(soPtrs.ToArray(), description);
            }

            return new SceneObject[0];
        }

        public static SceneObject CloneSO(SceneObject so, string description = "")
        {
            if (so != null)
                return Internal_CloneSO(so.GetCachedPtr(), description);

            return null;
        }

        public static SceneObject Instantiate(Prefab prefab, string description = "")
        {
            if (prefab != null)
                return Internal_Instantiate(prefab.GetCachedPtr(), description);

            return null;
        }

        public static SceneObject CreateSO(string name, string description = "")
        {
            return Internal_CreateSO(name, description);
        }

        public static void DeleteSO(SceneObject so, string description = "")
        {
            if (so != null)
                Internal_DeleteSO(so.GetCachedPtr(), description);
        }

        public static void ReparentSO(SceneObject so, SceneObject parent, string description = "")
        {
            if (so != null)
            {
                IntPtr parentPtr = IntPtr.Zero;
                if (parent != null)
                    parentPtr = parent.GetCachedPtr();

                Internal_ReparentSO(so.GetCachedPtr(), parentPtr, description);
            }
        }

        public static void ReparentSO(SceneObject[] so, SceneObject parent, string description = "")
        {
            if (so != null)
            {
                List<IntPtr> soPtrs = new List<IntPtr>();
                for (int i = 0; i < so.Length; i++)
                {
                    if (so[i] != null)
                        soPtrs.Add(so[i].GetCachedPtr());
                }

                if (soPtrs.Count > 0)
                {
                    IntPtr parentPtr = IntPtr.Zero;
                    if (parent != null)
                        parentPtr = parent.GetCachedPtr();

                    Internal_ReparentSOMulti(soPtrs.ToArray(), parentPtr, description);
                }
            }
        }

        public static void PushGroup(string name)
        {
            Internal_PushGroup(name);
        }

        public static void PopGroup(string name)
        {
            Internal_PopGroup(name);
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_Undo();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_Redo();

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_PushGroup(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_PopGroup(string name);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_RecordSO(IntPtr soPtr, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneObject Internal_CloneSO(IntPtr soPtr, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneObject[] Internal_CloneSOMulti(IntPtr[] soPtr, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneObject Internal_Instantiate(IntPtr prefabPtr, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern SceneObject Internal_CreateSO(string name, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_DeleteSO(IntPtr soPtr, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ReparentSO(IntPtr soPtr, IntPtr parentSOPtr, string description);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Internal_ReparentSOMulti(IntPtr[] soPtr, IntPtr parentSOPtr, string description);
    }
}
