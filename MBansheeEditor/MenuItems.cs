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
        [MenuItem("Components/Camera", 7050)]
        private static void AddCamera()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Camera component");
            so.AddComponent<Camera>();
        }

        [MenuItem("Components/Renderable", 7049)]
        private static void AddRenderable()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Renderable component");
            so.AddComponent<Renderable>();
        }

        [MenuItem("Components/Point light", 7048)]
        private static void AddPointLight()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Point;
        }

        [MenuItem("Components/Spot light", 7047)]
        private static void AddSpotLight()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Spot;
        }

        [MenuItem("Components/Directional light", 7046)]
        private static void AddDirectionalLight()
        {
            SceneObject so = Selection.sceneObject;
            if (so == null)
                return;

            UndoRedo.RecordSO(so, "Added a Light component");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;
        }

        [MenuItem("Scene Objects/Camera", 8050)]
        private static void AddCameraSO()
        {
            SceneObject so = UndoRedo.CreateSO("Camera", "Created a Camera");
            so.AddComponent<Camera>();

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Renderable", 8049)]
        private static void AddRenderableSO()
        {
            SceneObject so = UndoRedo.CreateSO("Renderable", "Created a Renderable");
            so.AddComponent<Renderable>();

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Point light", 8048)]
        private static void AddPointLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Point light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Point;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Spot light", 8047)]
        private static void AddSpotLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Spot light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Spot;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/Directional light", 8046)]
        private static void AddDirectionalLightSO()
        {
            SceneObject so = UndoRedo.CreateSO("Directional light", "Created a Light");
            Light light = so.AddComponent<Light>();
            light.Type = LightType.Directional;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/3D primitives/Box", 8100)]
        private static void Add3DBox()
        {
            SceneObject so = UndoRedo.CreateSO("Box", "Created a box");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Box;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/3D primitives/Sphere", 8099)]
        private static void Add3DSphere()
        {
            SceneObject so = UndoRedo.CreateSO("Sphere", "Created a sphere");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Sphere;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/3D primitives/Cone", 8098)]
        private static void Add3DCone()
        {
            SceneObject so = UndoRedo.CreateSO("Cone", "Created a cone");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Cone;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/3D primitives/Quad", 8097)]
        private static void Add3DQuad()
        {
            SceneObject so = UndoRedo.CreateSO("Quad", "Created a quad");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Quad;

            Selection.sceneObject = so;
        }

        [MenuItem("Scene Objects/3D primitives/Disc", 8096)]
        private static void Add3DDisc()
        {
            SceneObject so = UndoRedo.CreateSO("Disc", "Created a disc");
            Renderable renderable = so.AddComponent<Renderable>();
            renderable.Mesh = Builtin.Disc;

            Selection.sceneObject = so;
        }
    }
}
