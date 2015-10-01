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

        protected GUIPanel GUI;
        protected GUILayoutY layout;
        protected object referencedObject;

        private GUIPanel RootGUI;

        /// <summary>
        /// Initializes the inspector. Must be called after construction.
        /// </summary>
        /// <param name="gui">GUI panel to add the GUI elements to.</param>
        /// <param name="instance">Instance of the object whose fields to display GUI for.</param>
        internal virtual void Initialize(GUIPanel gui, object instance)
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

            Initialize();
            Refresh();
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
        /// Called when the inspector is first created.
        /// </summary>
        protected internal abstract void Initialize();

        /// <summary>
        /// Checks if contents of the inspector have been modified, and updates them if needed.
        /// </summary>
        /// <returns>True if there were any modifications, false otherwise.</returns>
        protected internal abstract bool Refresh();
    }
}
