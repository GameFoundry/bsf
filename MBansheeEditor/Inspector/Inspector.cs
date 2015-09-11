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

        internal virtual void Initialize(InspectorWindow parentWindow, GUIPanel gui, object instance)
        {
            GUI = gui;
            layout = GUI.AddLayoutY();
            referencedObject = instance;
            this.parentWindow = parentWindow;
        }

        internal virtual void SetVisible(bool visible)
        {
            GUI.Visible = visible;
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
