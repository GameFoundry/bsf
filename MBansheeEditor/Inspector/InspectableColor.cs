using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a color. Color is displayed as a GUI color field that allows
    /// the user to manipulate the color using a color picker.
    /// </summary>
    public class InspectableColor : InspectableField
    {
        private Color propertyValue;
        private GUIColorField guiField;
        private bool isInitialized;

        /// <summary>
        /// Creates a new inspectable color GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableColor(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <summary>
        /// Initializes the GUI elements the first time <see cref="Update"/> gets called.
        /// </summary>
        /// <param name="layoutIndex">Index at which to insert the GUI elements.</param>
        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Color)
            {
                guiField = new GUIColorField(new GUIContent(title));
                guiField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiField);
            }

            isInitialized = true;
        }

        /// <inheritdoc/>
        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            Color newPropertyValue = property.GetValue<Color>();
            if (propertyValue != newPropertyValue)
                return true;

            return base.IsModified();
        }

        /// <inheritdoc/>
        protected override void Update(int layoutIndex)
        {
            base.Update(layoutIndex);

            if (!isInitialized)
                Initialize(layoutIndex);

            // TODO - Skip update if it currently has input focus so user can modify the value in peace

            propertyValue = property.GetValue<Color>();
            if (guiField != null)
                guiField.Value = propertyValue;
        }

        /// <summary>
        /// Triggered when the user selects a new color.
        /// </summary>
        /// <param name="newValue">New value of the color field.</param>
        private void OnFieldValueChanged(Color newValue)
        {
            property.SetValue(newValue);
        }
    }
}
