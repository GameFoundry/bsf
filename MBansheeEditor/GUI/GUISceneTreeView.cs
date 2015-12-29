using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.CompilerServices;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// GUI element that displays all scene objects in the current scene as a tree view.
    /// </summary>
    public sealed class GUISceneTreeView : GUIElement
    {
        /// <summary>
        /// Creates a new scene tree view element.
        /// </summary>
        /// <param name="style">Optional style to use for the element. Style controls the look of the element, as well as 
        ///                     default layout options. Style will be retrieved from the active GUISkin. If not specified 
        ///                     default element style is used.</param>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUISceneTreeView(string style = "", params GUIOption[] options)
        {
            Internal_CreateInstance(this, style, options);
        }

        /// <summary>
        /// Creates a new scene tree view element.
        /// </summary>
        /// <param name="options">Options that allow you to control how is the element  positioned and sized. This will 
        ///                       override any similar options set by style.</param>
        public GUISceneTreeView(params GUIOption[] options)
        {
            Internal_CreateInstance(this, "", options);
        }

        /// <summary>
        /// Updates the contents of the tree view with most recent scene data. Should be called once per frame.
        /// </summary>
        public void Update()
        {
            Internal_Update(mCachedPtr);
        }

        /// <summary>
        /// Triggered by the runtime when the scene is modified from the native scene tree view.
        /// </summary>
        private void Internal_DoOnModified()
        {
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Triggered by the runtime when a resource is dropped on the scene tree view.
        /// </summary>
        private void Internal_DoOnResourceDropped(SceneObject parent, string[] resourcePaths)
        {
            if (resourcePaths == null)
                return;

            List<SceneObject> addedObjects = new List<SceneObject>(); 
            for (int i = 0; i < resourcePaths.Length; i++)
            {
                LibraryEntry entry = ProjectLibrary.GetEntry(resourcePaths[i]);
                if (entry != null && entry.Type == LibraryEntryType.File)
                {
                    FileEntry fileEntry = (FileEntry)entry;
                    if (fileEntry.ResType == ResourceType.Mesh)
                    {
                        if (!string.IsNullOrEmpty(resourcePaths[i]))
                        {
                            string meshName = Path.GetFileNameWithoutExtension(resourcePaths[i]);

                            Mesh mesh = ProjectLibrary.Load<Mesh>(resourcePaths[i]);
                            if (mesh == null)
                                continue;

                            SceneObject so = UndoRedo.CreateSO(meshName, "Created a new Renderable \"" + meshName + "\"");
                            so.Parent = parent;

                            Renderable renderable = so.AddComponent<Renderable>();
                            renderable.Mesh = mesh;

                            addedObjects.Add(so);
                        }
                    }
                    else if (fileEntry.ResType == ResourceType.Prefab)
                    {
                        if (!string.IsNullOrEmpty(resourcePaths[i]))
                        {
                            Prefab prefab = ProjectLibrary.Load<Prefab>(resourcePaths[i]);
                            SceneObject so = UndoRedo.Instantiate(prefab, "Instantiating " + prefab.Name);
                            so.Parent = parent;

                            addedObjects.Add(so);
                        }
                    }
                }
            }

            if(addedObjects.Count > 0)
                EditorApplication.SetSceneDirty();

            Selection.SceneObjects = addedObjects.ToArray();
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_CreateInstance(GUISceneTreeView instance, string style, GUIOption[] options);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void Internal_Update(IntPtr thisPtr);
    }
}
