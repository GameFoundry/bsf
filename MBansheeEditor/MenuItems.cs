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

            UndoRedo.RecordSO(so, "Added a Camera component");
            so.AddComponent<Camera>();
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

            UndoRedo.RecordSO(so, "Added a Renderable component");
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

            UndoRedo.RecordSO(so, "Added a Light component");
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

            UndoRedo.RecordSO(so, "Added a Light component");
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

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new empty scene object.
        /// </summary>
        [MenuItem("Scene Objects/Scene Object", 8051)]
        [ToolbarItem("SceneObject", ToolbarIcon.NewSceneObject, "", 1601, true)]
        private static void AddEmptySO()
        {
            SceneObject so = UndoRedo.CreateSO("SceneObject", "Created an empty SceneObject");

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a camera component.
        /// </summary>
        [MenuItem("Scene Objects/Camera", 8050)]
        [ToolbarItem("Camera", ToolbarIcon.NewCamera, "", 1600, false)]
        private static void AddCameraSO()
        {
            SceneObject so = UndoRedo.CreateSO("Camera", "Created a Camera");
            so.AddComponent<Camera>();

            Selection.SceneObject = so;
            EditorApplication.SetSceneDirty();
        }

        /// <summary>
        /// Creates a new scene object with a renderable component.
        /// </summary>
        [MenuItem("Scene Objects/Renderable", 8049)]
        [ToolbarItem("Renderable", ToolbarIcon.NewRenderable, "", 1599)]
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
        [ToolbarItem("Point light", ToolbarIcon.NewPointLight, "", 1598)]
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
        [ToolbarItem("Spot light", ToolbarIcon.NewSpotLight, "", 1597)]
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
        [ToolbarItem("Directional light", ToolbarIcon.NewDirLight, "", 1596)]
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
        [ToolbarItem("Cube", ToolbarIcon.NewCube, "", 1700, true)]
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
        [ToolbarItem("Sphere", ToolbarIcon.NewSphere, "", 1699)]
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
        [ToolbarItem("Cone", ToolbarIcon.NewCone, "", 1698)]
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
        [ToolbarItem("Quad", ToolbarIcon.NewQuad, "", 1697)]
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
    }
}
