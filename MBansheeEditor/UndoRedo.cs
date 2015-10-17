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
    /// Provides functionality to undo or redo recently performed operations in the editor.
    /// </summary>
    public static class UndoRedo
    {
        /// <summary>
        /// Returns the unique identifier of the command currently at the top of the undo stack.
        /// </summary>
        public static int TopCommandId
        {
            get { return Internal_GetTopCommandId(); }
        }

        /// <summary>
        /// Executes the last command on the undo stack, undoing its operations.
        /// </summary>
        [MenuItem("Edit/Undo", 9500, true)]
        [ToolbarItem("Undo", ToolbarIcon.Undo, "", 1900, true)]
        public static void Undo()
        {
            Internal_Undo();
        }

        /// <summary>
        /// Executes the last command on the redo stack (last command we called undo on), re-applying its operation.
        /// </summary>
        [MenuItem("Edit/Redo", 9499, true)]
        [ToolbarItem("Redo", ToolbarIcon.Redo, "", 1899)]
        public static void Redo()
        {
            Internal_Redo();
        }

        /// <summary>
        /// Records a state of the entire scene object at a specific point and allows you to restore it to its original 
        /// values as needed.
        /// </summary>
        /// <param name="so">Scene object to record.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
        public static void RecordSO(SceneObject so, string description = "")
        {
            if (so != null)
                Internal_RecordSO(so.GetCachedPtr(), description);
        }

        /// <summary>
        /// Creates new scene object(s) by cloning existing objects.
        /// </summary>
        /// <param name="so">Scene object(s) to clone.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
        /// <returns>Cloned scene objects.</returns>
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

        /// <summary>
        /// Creates new a scene object by cloning an existing object.
        /// </summary>
        /// <param name="so">Scene object to clone.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
        /// <returns>Cloned scene object.</returns>
        public static SceneObject CloneSO(SceneObject so, string description = "")
        {
            if (so != null)
                return Internal_CloneSO(so.GetCachedPtr(), description);

            return null;
        }

        /// <summary>
        /// Instantiates scene object(s) from a prefab.
        /// </summary>
        /// <param name="prefab">Prefab to instantiate.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
        /// <returns>Instantiated scene object.</returns>
        public static SceneObject Instantiate(Prefab prefab, string description = "")
        {
            if (prefab != null)
                return Internal_Instantiate(prefab.GetCachedPtr(), description);

            return null;
        }

        /// <summary>
        /// Creates a new scene object.
        /// </summary>
        /// <param name="name">Name of the scene object.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
        /// <returns>Newly created scene object.</returns>
        public static SceneObject CreateSO(string name, string description = "")
        {
            return Internal_CreateSO(name, description);
        }

        /// <summary>
        /// Deletes a scene object.
        /// </summary>
        /// <param name="so">Scene object to delete.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
        public static void DeleteSO(SceneObject so, string description = "")
        {
            if (so != null)
                Internal_DeleteSO(so.GetCachedPtr(), description);
        }

        /// <summary>
        /// Changes the parent of the scene object.
        /// </summary>
        /// <param name="so">Scene object to change the parent of.</param>
        /// <param name="parent">New parent.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
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

        /// <summary>
        /// Changes the parent of a set of scene objects.
        /// </summary>
        /// <param name="so">Scene objects to change the parent of.</param>
        /// <param name="parent">New parent.</param>
        /// <param name="description">Optional description of what exactly the command does.</param>
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

        /// <summary>
        /// Creates a new undo/redo group. All new commands will be registered to this group. You may remove the group and 
        /// all of its commands by calling <see cref="PopGroup"/>.
        /// </summary>
        /// <param name="name">Unique name of the group.</param>
        public static void PushGroup(string name)
        {
            Internal_PushGroup(name);
        }

        /// <summary>
        /// Removes all the command registered to the current undo/redo group.
        /// </summary>
        /// <param name="name">Unique name of the group.</param>
        public static void PopGroup(string name)
        {
            Internal_PopGroup(name);
        }

        /// <summary>
        /// Removes a command with the specified identifier from undo/redo stack without executing it.
        /// </summary>
        /// <param name="id">Identifier of the command as returned by <see cref="GetTopCommandId"/></param>
        public static void PopCommand(int id)
        {
            Internal_PopCommand(id);
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
        internal static extern void Internal_PopCommand(int id);

        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern int Internal_GetTopCommandId();

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
