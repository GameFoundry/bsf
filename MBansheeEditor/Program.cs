using System;
using System.Collections.Generic;
using BansheeEngine;

namespace BansheeEditor
{
    class ProgramEd
    {
        private static InspectorWindow window;
        private static Debug_Component1 dbgComponent;

        static void Main()
        {
            window = EditorWindow.OpenWindow<InspectorWindow>();

            SceneObject newDbgObject = new SceneObject("NewDbgObject");
            dbgComponent = newDbgObject.AddComponent<Debug_Component1>();
            newDbgObject.AddComponent<Debug_Component2>();

            window.SetObjectToInspect(newDbgObject);

            DbgResource testResource = new DbgResource();
            //ProjectLibrary.Create(testResource, @"D:\DummyBansheeProject\Resources\testResource");



            // Starts main editor window with the specified width/height and render system
            // (Window position and sizes are stored internally. Restored upon StartUp and saved upon ShutDown)
            //EditorApplication.StartUp(RenderSystem.DX11, 1024, 800);

            // TODO - A class to manipulate menu items

            //ProjectSelectWindow window = new ProjectSelectWindow();

            //GUIElementStateStyle dbgStyle = new GUIElementStateStyle();
            //Color newColor = Color.red;

            //dbgStyle.textColor = newColor;
        }

        static void EditorUpdate()
        {
            window.Refresh();

            if (dbgComponent != null)
                dbgComponent.intArray[0] = dbgComponent.intArray[0] + 1;
        }
    }

    //class SceneView : EditorWindow
    //{
       // SceneView()
    //    {
            // GUI is a GUILayout that is initialized internally by EditorWindow
            //GUI.AddButton("Some text");
            //GUI.AddXLayout();
            //GUI.CreateAreaFixed(100, 100, 400, 400); // GUIArea creating another GUIArea

            // Add support for GUIContent
            // Add support for GUILayoutOptions
            // Add support for GUIElementStyle and GUISkin - For now ignore this as I need to deal with Fonts, Textures, SpriteTextures, etc.
     //   }
    //}

    //class MyGUI : //GUIWidget // GUIWidget is automatically initialized with Game render window. Editor GUIWidgets are specially initialized 
    //{
        
    //}
}
