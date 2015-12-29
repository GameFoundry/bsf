using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Contains various menu item callbacks for the main editor menu bar.
    /// </summary>
    static class MenuItems
    {
        /// <summary>
        /// Adds a camera component to the currently selected scene object.
        /// </summary>
        [MenuItem("Components/Camera", 7050)]
        private static void AddCamera()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, false, "Added a Camera component");
            Camera cam = so.AddComponent<Camera>();
            cam.Main = true;

            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Adds a renderable component to the currently selected scene object.
        /// </summary>
        [MenuItem("Components/Renderable", 7049)]
        private static void AddRenderable()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, false, "Added a Renderable component");
            so.AddComponent<Renderable>();
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Adds a point light component to the currently selected scene object.
        /// </summary>
        [MenuItem("Components/Point light", 7048)]
        private static void AddPointLight()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, false, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Point;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Adds a spot light component to the currently selected scene object.
        /// </summary>
        [MenuItem("Components/Spot light", 7047)]
        private static void AddSpotLight()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, false, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Spot;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Adds a directional light component to the currently selected scene object.
        /// </summary>
        [MenuItem("Components/Directional light", 7046)]
        private static void AddDirectionalLight()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, false, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new empty scene object.
        /// </summary>
        [MenuItem("Scene Objects/Scene Object", 8051)]
        [ToolbarItem("SceneObject", ToolbarIcon.NewSceneObject, "Creates a new empty scene object.", 1601, true)]
        private static void AddEmptySO()
        {
            SceneObject so = UndoRedo.CreateSO("SceneObject", "New scene object");

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a camera component.
        /// </summary>
        [MenuItem("Scene Objects/Camera", 8050)]
        [ToolbarItem("Camera", ToolbarIcon.NewCamera, "New camera", 1600, false)]
        private static void AddCameraSO()
        {
            SceneObject so = UndoRedo.CreateSO("Camera", "Created a Camera");
            Camera cam = so.AddComponent<Camera>();
            cam.Main = true;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a renderable component.
        /// </summary>
        [MenuItem("Scene Objects/Renderable", 8049)]
        [ToolbarItem("Renderable", ToolbarIcon.NewRenderable, "New renderable", 1599)]
        private static void AddRenderableSO()
        {
            SceneObject so = UndoRedo.CreateSO("Renderable", "Created a Renderable");
            so.AddComponent<Renderable>();

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a point light component.
        /// </summary>
        [MenuItem("Scene Objects/Point light", 8048)]
        [ToolbarItem("Point light", ToolbarIcon.NewPointLight, "New point light", 1598)]
        private static void AddPointLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Point light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Point;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a spot light component.
        /// </summary>
        [MenuItem("Scene Objects/Spot light", 8047)]
        [ToolbarItem("Spot light", ToolbarIcon.NewSpotLight, "New spot light", 1597)]
        private static void AddSpotLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Spot light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Spot;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a directional light component.
        /// </summary>
        [MenuItem("Scene Objects/Directional light", 8046)]
        [ToolbarItem("Directional light", ToolbarIcon.NewDirLight, "New directional light.", 1596)]
        private static void AddDirectionalLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Directional light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a box primitive.
        /// </summary>
        [MenuItem("Scene Objects/3D primitives/Box", 8100)]
        [ToolbarItem("Cube", ToolbarIcon.NewCube, "Creates a scene object with a box primitive.", 1700, true)]
        private static void Add3DBox()
        {
            SceneObject so = UndoRedo.CreateSO("Box", "Created a box");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Box;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a sphere primitive.
        /// </summary>
        [MenuItem("Scene Objects/3D primitives/Sphere", 8099)]
        [ToolbarItem("Sphere", ToolbarIcon.NewSphere, "Creates a scene object with a sphere primitive.", 1699)]
        private static void Add3DSphere()
        {
            SceneObject so = UndoRedo.CreateSO("Sphere", "Created a sphere");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Sphere;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a cone primitive.
        /// </summary>
        [MenuItem("Scene Objects/3D primitives/Cone", 8098)]
        [ToolbarItem("Cone", ToolbarIcon.NewCone, "Creates a scene object with a cone primitive.", 1698)]
        private static void Add3DCone()
        {
            SceneObject so = UndoRedo.CreateSO("Cone", "Created a cone");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Cone;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a quad primitive.
        /// </summary>
        [MenuItem("Scene Objects/3D primitives/Quad", 8097)]
        [ToolbarItem("Quad", ToolbarIcon.NewQuad, "Creates a scene object with a quad primitive.", 1697)]
        private static void Add3DQuad()
        {
            SceneObject so = UndoRedo.CreateSO("Quad", "Created a quad");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Quad;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a disc primitive.
        /// </summary>
        [MenuItem("Scene Objects/3D primitives/Disc", 8096)]
        private static void Add3DDisc()
        {
            SceneObject so = UndoRedo.CreateSO("Disc", "Created a disc");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Disc;

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Applies changes from the prefab instance to the prefab resource.
        /// </summary>
        [MenuItem("Scene Objects/Apply prefab", 8025, true)]
        private static void ApplyPrefab()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            PrefabUtility.ApplyPrefab(so);
        }

        /// <summary>
        /// Reverts a prefab instance to the original state of its prefab.
        /// </summary>
        [MenuItem("Scene Objects/Revert to prefab", 8024)]
        private static void RevertToPrefab()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, true, "Reverting \"" + so.Name + "\" to prefab.");

            PrefabUtility.RevertPrefab(so);
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Breaks a link between a prefab and its instance.
        /// </summary>
        [MenuItem("Scene Objects/Break prefab link", 8023)]
        private static void BreakPrefabLink()
        {
            SceneObject so = Selection.SceneObject;
            if (so == null)
                return;

            UndoRedo.BreakPrefab(so, "Breaking prefab link for " + so.Name);
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Cuts the currently selected scene object or resource.
        /// </summary>
        [MenuItem("Edit/Cut", 9450, true)]
        public static void Cut()
        {
            if (Selection.SceneObjects != null && Selection.SceneObjects.Length > 0)
            {
                HierarchyWindow win = EditorWindow.GetWindow<HierarchyWindow>();
                if (win != null)
                    win.CutSelection();
            }
            else if (Selection.ResourcePaths != null && Selection.ResourcePaths.Length > 0)
            {
                LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
                if (win != null)
                    win.CutSelection();
            }
        }

        /// <summary>
        /// Copies the currently selected scene object or resource.
        /// </summary>
        [MenuItem("Edit/Copy", 9449)]
        public static void Copy()
        {
            if (Selection.SceneObjects != null && Selection.SceneObjects.Length > 0)
            {
                HierarchyWindow win = EditorWindow.GetWindow<HierarchyWindow>();
                if (win != null)
                    win.CopySelection();
            }
            else if (Selection.ResourcePaths != null && Selection.ResourcePaths.Length > 0)
            {
                LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
                if (win != null)
                    win.CopySelection();
            }
        }

        /// <summary>
        /// Pastes the scene objects or resources that were previously cut or copied.
        /// </summary>
        [MenuItem("Edit/Paste", 9448)]
        public static void Paste()
        {
            // TODO - This is slightly wrong in case both windows have something in their paste buffer (unify them?)

            {
                HierarchyWindow win = EditorWindow.GetWindow<HierarchyWindow>();
                if (win != null)
                    win.PasteToSelection();
            }

            {
                LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
                if (win != null)
                    win.PasteToSelection();
            }
        }

        /// <summary>
        /// Deletes currently selected scene objects or resources.
        /// </summary>
        [MenuItem("Edit/Delete", 9447)]
        public static void Delete()
        {
            if (Selection.SceneObjects != null && Selection.SceneObjects.Length > 0)
            {
                HierarchyWindow win = EditorWindow.GetWindow<HierarchyWindow>();
                if (win != null)
                    win.DeleteSelection();
            }
            else if (Selection.ResourcePaths != null && Selection.ResourcePaths.Length > 0)
            {
                LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
                if (win != null)
                    win.DeleteSelection();
            }
        }

        /// <summary>
        /// Duplicates currently selected scene objects or resources.
        /// </summary>
        [MenuItem("Edit/Duplicate", 9446)]
        public static void Duplicate()
        {
            if (Selection.SceneObjects != null && Selection.SceneObjects.Length > 0)
            {
                HierarchyWindow win = EditorWindow.GetWindow<HierarchyWindow>();
                if (win != null)
                    win.DuplicateSelection();
            }
            else if (Selection.ResourcePaths != null && Selection.ResourcePaths.Length > 0)
            {
                LibraryWindow win = EditorWindow.GetWindow<LibraryWindow>();
                if (win != null)
                    win.DuplicateSelection();
            }
        }
    }
}
