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
        protected GUILayout layout;
        protected object referencedObject;

        internal void Initialize(GUIPanel gui, object instance)
        {
            GUI = gui;
            layout = gui.layout.AddLayoutY();
            referencedObject = instance;
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
            GUI.Destroy();
        }

        internal abstract void Refresh();
        internal abstract int GetOptimalHeight();
    }
}
