using System.Collections.Generic;
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
        private Dictionary<string, object> persistent;

        /// <summary>
        /// Initializes the inspector. Must be called after construction.
        /// </summary>
        /// <param name="gui">GUI panel to add the GUI elements to.</param>
        /// <param name="instance">Instance of the object whose fields to display GUI for.</param>
        /// <param name="persistent">A set of properties that the inspector can read/write. They will be persisted for the 
        ///                          inspector even after it is closed.</param>
        internal virtual void Initialize(GUIPanel gui, object instance, Dictionary<string, object> persistent)
        {
            rootGUI = gui;
            this.persistent = persistent;

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
            rootGUI.Active = visible;
        }

        /// <summary>
        /// Sets a persistent floating point property that will remain available for the inspector of this object, even
        /// after it is closed and re-opened.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        protected internal void SetFloat(string name, float value)
        {
            persistent[name] = value;
        }

        /// <summary>
        /// Sets a persistent integer property that will remain available for the inspector of this object, even after it 
        /// is closed and re-opened.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        protected internal void SetInt(string name, int value)
        {
            persistent[name] = value;
        }

        /// <summary>
        /// Sets a persistent boolean property that will remain available for the inspector of this object, even after it 
        /// is closed and re-opened.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        protected internal void SetBool(string name, bool value)
        {
            persistent[name] = value;
        }

        /// <summary>
        /// Sets a persistent string property that will remain available for the inspector of this object, even after it is
        /// closed and re-opened.
        /// </summary>
        /// <param name="name">Name to record the property under.</param>
        /// <param name="value">Value of the property.</param>
        protected internal void SetString(string name, string value)
        {
            persistent[name] = value;
        }

        /// <summary>
        /// Retrieves a persistent floating point property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        protected internal float GetFloat(string name, float defaultValue = 0.0f)
        {
            object value;
            if (persistent.TryGetValue(name, out value))
            {
                if (value is float)
                    return (float)value;
            }

            return defaultValue;
        }

        /// <summary>
        /// Retrieves a persistent integer property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        protected internal int GetInt(string name, int defaultValue = 0)
        {
            object value;
            if (persistent.TryGetValue(name, out value))
            {
                if (value is int)
                    return (int)value;
            }

            return defaultValue;
        }

        /// <summary>
        /// Retrieves a persistent boolean property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        protected internal bool GetBool(string name, bool defaultValue = false)
        {
            object value;
            if (persistent.TryGetValue(name, out value))
            {
                if (value is bool)
                    return (bool)value;
            }

            return defaultValue;
        }

        /// <summary>
        /// Retrieves a persistent string property.
        /// </summary>
        /// <param name="name">Name of the property to retrieve.</param>
        /// <param name="defaultValue">Default value to return if property cannot be found.</param>
        /// <returns>Value of the property if it exists, otherwise the default value.</returns>
        protected internal string GetString(string name, string defaultValue = "")
        {
            object value;
            if (persistent.TryGetValue(name, out value))
            {
                if (value is string)
                    return (string)value;
            }

            return defaultValue;
        }

        /// <summary>
        /// Checks does a persistent property with the specified name exists.
        /// </summary>
        /// <param name="name">Name of the property to check.</param>
        /// <returns>True if the property exists, false otherwise.</returns>
        protected internal bool HasKey(string name)
        {
            return persistent.ContainsKey(name);
        }

        /// <summary>
        /// Deletes a persistent property with the specified name.
        /// </summary>
        /// <param name="name">Name of the property to delete.</param>
        protected internal void DeleteKey(string name)
        {
            persistent.Remove(name);
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
        /// <returns>State representing was anything modified between two last calls to <see cref="Refresh"/>.</returns>
        protected internal abstract InspectableState Refresh();
    }
}
