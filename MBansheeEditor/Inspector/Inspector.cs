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
        protected object referencedObject;

        internal void Initialize(GUIPanel gui, object instance)
        {
            GUI = gui;
            referencedObject = instance;
        }

        internal void SetArea(int x, int y, int width, int height)
        {
            GUI.SetArea(x, y, width, height);
        }

        internal void Destroy()
        {
            GUI.Destroy();
        }

        internal abstract void Refresh();
        internal abstract int GetOptimalHeight();
    }
}
