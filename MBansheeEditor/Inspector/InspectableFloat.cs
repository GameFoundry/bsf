using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BansheeEngine;

namespace BansheeEditor
{
    /// <summary>
    /// Displays GUI for a serializable property containing a floating point value.
    /// </summary>
    public class InspectableFloat : InspectableField
    {
        private float propertyValue;
        private GUIFloatField guiFloatField;
        private bool isInitialized;

        /// <summary>
        /// Creates a new inspectable float GUI for the specified property.
        /// </summary>
        /// <param name="title">Name of the property, or some other value to set as the title.</param>
        /// <param name="depth">Determines how deep within the inspector nesting hierarchy is this field. Some fields may
        ///                     contain other fields, in which case you should increase this value by one.</param>
        /// <param name="layout">Parent layout that all the field elements will be added to.</param>
        /// <param name="property">Serializable property referencing the array whose contents to display.</param>
        public InspectableFloat(string title, int depth, InspectableFieldLayout layout, SerializableProperty property)
            : base(title, depth, layout, property)
        {

        }

        /// <summary>
        /// Initializes the GUI elements the first time <see cref="Update"/> gets called.
        /// </summary>
        /// <param name="layoutIndex">Index at which to insert the GUI elements.</param>
        private void Initialize(int layoutIndex)
        {
            if (property.Type == SerializableProperty.FieldType.Float)
            {
                guiFloatField = new GUIFloatField(new GUIContent(title));
                guiFloatField.OnChanged += OnFieldValueChanged;

                layout.AddElement(layoutIndex, guiFloatField);
            }

            isInitialized = true;
        }

        /// <inheritdoc/>
        protected override bool IsModified()
        {
            if (!isInitialized)
                return true;

            float newPropertyValue = property.GetValue<float>();
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

            propertyValue = property.GetValue<float>();
            if (guiFloatField != null)
            {
                if (guiFloatField.HasInputFocus())
                    return;

                guiFloatField.Value = propertyValue;
            }
        }

        /// <summary>
        /// Triggered when the user inputs a new floating point value.
        /// </summary>
        /// <param name="newValue">New value of the float field.</param>
        private void OnFieldValueChanged(float newValue)
        {
            property.SetValue(newValue);
        }
    }
}
