using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public abstract class Inspector
    {
        protected GUIPanel GUI;
        protected GUILayoutY layout;
        protected object referencedObject;

        private InspectorWindow parentWindow;

        internal void Initialize(InspectorWindow parentWindow, GUIPanel gui, object instance)
        {
            GUI = gui;
            layout = gui.layout.AddLayoutY();
            referencedObject = instance;
            this.parentWindow = parentWindow;
        }

        internal void SetArea(int x, int y, int width, int height)
        {
            GUI.SetArea(x, y, width, height);
        }

        internal void SetVisible(bool visible)
        {
            GUI.SetVisible(visible);
        }

        internal void Destroy()
        {
            layout.Destroy();
            parentWindow.DestroyPanel(GUI);
        }

        internal abstract bool Refresh();
        internal abstract int GetOptimalHeight();
    }
}
