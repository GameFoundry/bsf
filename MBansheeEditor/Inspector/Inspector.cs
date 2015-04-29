using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    public abstract class Inspector
    {
        protected GUIPanelNEW GUI;
        protected GUILayoutY layout;
        protected object referencedObject;

        private InspectorWindow parentWindow;

        internal void Initialize(InspectorWindow parentWindow, GUIPanelNEW gui, object instance)
        {
            GUI = gui;
            layout = gui.AddLayoutY();
            referencedObject = instance;
            this.parentWindow = parentWindow;
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

        internal abstract bool Refresh();
        internal abstract int GetOptimalHeight();
    }
}
