using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI elements for all the inspectable fields in an object.
    /// </summary>
    public abstract class Inspector
    {
        public const short START_BACKGROUND_DEPTH = 50;

        protected GUIPanel RootGUI;
        protected GUIPanel GUI;
        protected GUILayoutY layout;
        protected object referencedObject;

        private InspectorWindow parentWindow;

        /// <summary>
        /// Initializes the inspector. Must be called after construction.
        /// </summary>
        /// <param name="parentWindow">Parent window to render the GUI to.</param>
        /// <param name="gui">GUI panel to add the GUI elements to.</param>
        /// <param name="instance">Instance of the object whose fields to display GUI for.</param>
        internal virtual void Initialize(InspectorWindow parentWindow, GUIPanel gui, object instance)
        {
            RootGUI = gui;

            GUILayout contentLayoutX = gui.AddLayoutX();
            contentLayoutX.AddSpace(5);
            GUILayout contentLayoutY = contentLayoutX.AddLayoutY();
            contentLayoutY.AddSpace(5);
            GUIPanel contentPanel = contentLayoutY.AddPanel();
            contentLayoutY.AddSpace(5);
            contentLayoutX.AddSpace(5);

            GUIPanel backgroundPanel = gui.AddPanel(START_BACKGROUND_DEPTH);
            GUITexture inspectorContentBg = new GUITexture(null, EditorStyles.InspectorContentBg);
            backgroundPanel.AddElement(inspectorContentBg);

            GUI = contentPanel;
            layout = GUI.AddLayoutY();
            referencedObject = instance;
            this.parentWindow = parentWindow;
        }

        /// <summary>
        /// Hides or shows the inspector GUI elements.
        /// </summary>
        /// <param name="visible">True to make the GUI elements visible.</param>
        internal virtual void SetVisible(bool visible)
        {
            RootGUI.Enabled = visible;
        }

        /// <summary>
        /// Destroys all inspector GUI elements.
        /// </summary>
        internal void Destroy()
        {
            layout.Destroy();
            GUI.Destroy();
        }

        /// <summary>
        /// Checks if contents of the inspector have been modified, and updates them if needed.
        /// </summary>
        /// <returns>True if there were any modifications, false otherwise.</returns>
        internal abstract bool Refresh();
    }
}
