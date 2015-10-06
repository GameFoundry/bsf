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

        /// <summary>
        /// Returns the main GUI layout for the inspector.
        /// </summary>
        protected GUILayoutY Layout
        {
            get { return layout; }
        }

        /// <summary>
        /// Returns the main GUI panel for the inspector. <see cref="Layout"/> is a child of this panel.
        /// </summary>
        protected GUIPanel GUI
        {
            get { return mainPanel; }
        }

        /// <summary>
        /// Returns the object the inspector is currently displaying.
        /// </summary>
        protected object InspectedObject
        {
            get { return inspectedObject; }
        }

        private GUIPanel rootGUI;
        private GUIPanel mainPanel;
        private GUILayoutY layout;
        private object inspectedObject;

        /// <summary>
        /// Initializes the inspector. Must be called after construction.
        /// </summary>
        /// <param name="gui">GUI panel to add the GUI elements to.</param>
        /// <param name="instance">Instance of the object whose fields to display GUI for.</param>
        internal virtual void Initialize(GUIPanel gui, object instance)
        {
            rootGUI = gui;

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

            mainPanel = contentPanel;
            layout = GUI.AddLayoutY();
            inspectedObject = instance;

            Initialize();
            Refresh();
        }

        /// <summary>
        /// Hides or shows the inspector GUI elements.
        /// </summary>
        /// <param name="visible">True to make the GUI elements visible.</param>
        internal virtual void SetVisible(bool visible)
        {
            rootGUI.Enabled = visible;
        }

        /// <summary>
        /// Destroys all inspector GUI elements.
        /// </summary>
        internal void Destroy()
        {
            Layout.Destroy();
            GUI.Destroy();
        }

        /// <summary>
        /// Called when the inspector is first created.
        /// </summary>
        protected internal abstract void Initialize();

        /// <summary>
        /// Checks if contents of the inspector have been modified, and updates them if needed.
        /// </summary>
        protected internal abstract void Refresh();
    }
}
